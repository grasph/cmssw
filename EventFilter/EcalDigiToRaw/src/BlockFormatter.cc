//Emacs settings:-*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil -*-
#include <memory>

// user include files


#include "EventFilter/EcalDigiToRaw/interface/EcalDigiToRaw.h"
#include "EventFilter/EcalDigiToRaw/interface/BlockFormatter.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/EcalDetId/interface/EcalElectronicsId.h"


using namespace std;

BlockFormatter::BlockFormatter(): base_(0) {
}

BlockFormatter::~BlockFormatter() {
}


void BlockFormatter::SetParam(EcalDigiToRaw* base) {
  base_ = base;
  pcounter_ = (base -> GetCounter());
  debug_   = base -> GetDebug();
  porbit_number_ = (base -> GetOrbit());
  plv1_ = (base -> GetLV1());
  pbx_  = (base -> GetBX());
  prunnumber_ = (base -> GetRunNumber());
  doBarrel_ = base -> GetDoBarrel();
  doEndCap_ = base -> GetDoEndCap();
  plistDCCId_ = base -> GetListDCCId();
  doTCC_ = base -> GetDoTCC();
  doSR_ = base -> GetDoSR();
  doTower_ = base -> GetDoTower();
}


void BlockFormatter::DigiToRaw(FEDRawDataCollection* productRawData){
  for (int idcc=1; idcc <= 54; idcc++) {
    int run_number = *prunnumber_;
    int orbit_number_= *porbit_number_;
    int bx = *pbx_;
    int lv1 = *plv1_;

    EcalDCCHeaderBlock dccHeader = base_->GetDCCHeader(idcc);

    run_number    = dccHeader.getRunNumber();
    orbit_number_ = dccHeader.getOrbit();
    bx            = dccHeader.getBX();
    lv1           = dccHeader.getLV1();

    if (debug_) cerr << "in BlockFormatter::DigiToRaw  idcc run_number orbit_number bx lv1 "
                     << dec << idcc << " " << run_number << " "
                     << orbit_number_ << " " << bx << " " << lv1 << endl;


    if ( (! doBarrel_) &&
         (idcc >= EcalElectronicsId::MIN_DCCID_EBM && idcc <= EcalElectronicsId::MAX_DCCID_EBP)) continue;
    if ( (! doEndCap_) &&
         (idcc <= EcalElectronicsId::MAX_DCCID_EEM || idcc >= EcalElectronicsId::MIN_DCCID_EEP)) continue;

    int FEDid = FEDNumbering::MINECALFEDID + idcc;
    FEDRawData& rawdata = productRawData -> FEDData(FEDid);
    unsigned char * pData;
    short int DCC_ERRORS = 0;

    if (rawdata.size() == 0) {
      rawdata.resize(8);
      pData = rawdata.data();

      const int fov = 7;
      const int dcc_fov = 1;

      Word64 word = 0x8LL /*0x18*/
        | (Word64(fov)       & 0xFLL)      <<4
        | (Word64(FEDid)     & 0xFFFLL)    <<8
        //	| (Word64(*pbx_)     & 0xFFFLL)    <<20
        | (Word64(bx)   & 0xFFFLL)         <<20
        | (Word64(lv1)       & 0xFFFFFFLL) <<32
        | 0x51LL           <<56;

      Word64* pw = reinterpret_cast<Word64*>(const_cast<unsigned char*>(pData));
      *pw = word;    // DAQ header

      // std::cerr << __FILE__ << ":" << __LINE__ << ": "
      //                << "rawdata[7] = 0x" << hex << int(((char*) rawdata.data())[7])
      //                << " pw = 0x" << (*pw)
      //                << " word = 0x" << word
      //                << " lv1 = 0x" << lv1
      //		<< dec << "\n";


      rawdata.resize(rawdata.size() + 8*8);   // DCC header
      pData = rawdata.data();
      pData[11] = DCC_ERRORS & 0xFF;
      pData[12] = run_number & 0xFF;
      pData[13] = (run_number >>8) & 0xFF;
      pData[14] = (run_number >> 16) & 0xFF;
      pData[15] = 0x01;

      for (int i=16; i <= 22; i++) {
        pData[i] = 0;    // to be filled for local data taking or calibration
      }

      //FIXME: only global Physics run currently supported
      //(DCC_ID = 0, RT_HALF = 3 /Full/, RT_TYPE = 5 /Global/
      // SEQ = 0 /Phys./, MPGA = 1 /Gain 12, default/)
      const unsigned run_type = 0x45c0;

      pData[16] = run_type & 0xFF;
      pData[17] = (run_type >>8) & 0xFF;

      //Calibration abort-gap trigger information
      //("detailed trigger type")
      int dtt_dccid = dccHeader.getDccInTCCCommand();
      int dtt_color = dccHeader.getEventSettings().wavelength;
      int dtt_type = (int)runType2dtt(run_type);
      int dtt_half = dccHeader.getRtHalf();

      int dtt = (dtt_dccid & 0x3F )
        | ((dtt_color & 0x3) << 6 )
        | ((dtt_type  & 0x7) << 8 )
        | ((dtt_half  & 0x1) << 11);

      pData[20] = dtt & 0xFF;
      pData[21] = (dtt >> 8) & 0xFF;
      pData[22] = dcc_fov;
      pData[23] = 0x02;
      pData[24] = orbit_number_ & 0xFF;
      pData[25] = (orbit_number_ >>8) & 0xFF;
      pData[26] = (orbit_number_ >>16) & 0xFF;
      pData[27] = (orbit_number_ >>24) & 0xFF;
      //      int SRenable_ = 1;

      //FIXME: this flag should better be stored in the EcalDCCHeaderBlock class,
      //as its value can be configured in the DCC. Normal configuration is
      //1 for DCC connected to some PN. Supports of this flag started with FOV > 1.
      int MF = 0;
      if(fov > 1
         && idcc !=  3 && idcc !=  4 && idcc !=  7 && idcc !=  8 && idcc !=  9
         && idcc != 48 && idcc != 49 && idcc != 52 && idcc != 53 && idcc != 54){
        MF = 1;
      }
      int SR = dccHeader.getSelectiveReadout();
      int ZS = dccHeader.getZeroSuppression();
      int TZS = dccHeader.getTestZeroSuppression();
      int SR_CHSTATUS = dccHeader.getSrpStatus();
      std::vector<short> tccStatus = dccHeader.getTccStatus();
      tccStatus.resize(4);
      pData[28] = (SR & 0x1) | ((ZS & 0x1) <<1) | ((TZS & 0x1) <<2)
        | ((MF & 0x1) << 3) | ((SR_CHSTATUS & 0xF) <<4 );
      pData[29] = (tccStatus[0] & 0xF) | (tccStatus[1]& 0xF) <<4;
      pData[30] = (tccStatus[2] & 0xF) | (tccStatus[3]& 0xF) <<4;
      pData[31] = 0x03;

      std::vector<short> feStatus = dccHeader.getFEStatus();
      const int nFeCh = 70;
      feStatus.resize(nFeCh);
      int iCh = 0;
      for (int i = 0; i <= 4; i++) {
        for (int j = 0; j < 7; j++) {
          pData[32 +8*i + j] = (feStatus[iCh] & 0xF) | ((feStatus[iCh + 1] & 0xF) <<4);
          iCh += 2;
        }
        pData[32 +8*i + 7] = 0x04 + i;
      }
    } // endif rawdatasize == 0
  } // loop on id

}



void BlockFormatter::print(FEDRawData& rawdata) {
  int size = rawdata.size();
  cout << "Print RawData  size " << dec << size << endl;
  unsigned char* pData = rawdata.data();

  int n = size/8;
  for (int i=0; i < n; i++) {
    for (int j=7; j>=0; j--) {
      if (8*i+j <= size) cout << hex << (int)pData[8*i+j] << " ";
    }
    cout << endl;
  }
}



void BlockFormatter::CleanUp(FEDRawDataCollection* productRawData,
                             map<int, map<int,int> >* FEDorder ) {


  for (int id=0; id < 36 + 18; id++) {
    if ( (! doBarrel_) && (id >= 9 && id <= 44)) continue;
    if ( (! doEndCap_) && (id <= 8 || id >= 45)) continue;

    int FEDid = FEDNumbering::MINECALFEDID + id +1;
    FEDRawData& rawdata = productRawData -> FEDData(FEDid);


    // ---- if raw need not be made for a given fed, set its size to empty and return
    if ( find( (*plistDCCId_).begin(), (*plistDCCId_).end(), (id+1) ) == (*plistDCCId_).end() )
      {
        rawdata.resize( 0 );
        continue;
      }

    // ---- Add the trailer word
    int lastline = rawdata.size();
    rawdata.resize( lastline + 8);
    unsigned char * pData = rawdata.data();
    int event_length = (lastline + 8) / 8;   // in 64 bits words

    pData[lastline+7] = 0xa0;
    // pData[lastline+4] = event_length & 0xFFFFFF;
    pData[lastline+4] = event_length & 0xFF;
    pData[lastline+5] = (event_length >> 8) & 0xFF;
    pData[lastline+6] = (event_length >> 16) & 0xFF;
    //    int event_status = 0;
    int event_status = 1; //value observed in real data
    pData[lastline+1] = event_status & 0x0F;
    int tts = 8; //TTS ready
    pData[lastline] = (tts & 0xF) <<4;

    // ---- Write the event length in the DCC header
    // pData[8] = event_length & 0xFFFFFF;
    pData[8] = event_length & 0xFF;
    pData[9] = (event_length >> 8) & 0xFF;
    pData[10] = (event_length >> 16) & 0xFF;

    map<int, map<int,int> >::iterator fen = FEDorder -> find(FEDid);

    bool FED_has_data = true;
    if (fen == FEDorder->end()) FED_has_data = false;
    if (debug_ && (! FED_has_data)) cout << " FEDid is not in FEDorder ! " << endl;

#if 0 //this is mixing up the DCC header for real data. channel status must be
      //read from EcalDCCHeaderBlock.
    if ( ! FED_has_data) {
      int ch_status = 7;
      for (int iFE=1; iFE <= 68; iFE++) {
        int irow = (iFE-1) / 14;
        int kval = ( (iFE-1) % 14) / 2;
        if (iFE % 2 ==1) pData[32 + 8*irow + kval] |= ch_status & 0xFF;
        else pData[32 + 8*irow + kval] |= ((ch_status <<4) & 0xFF);
      }
    }

    if (FED_has_data) {
      map<int, int>& FEorder = (*fen).second;

      for (int iFE=1; iFE <= 68; iFE++) {
        map<int,int>::iterator fe = FEorder.find(iFE);
        int ch_status = 0;
        if (fe == FEorder.end())	// FE not present due to SRP, update CH_status
          ch_status = 7;		// CH_SUPPRESS
        int irow = (iFE-1) / 14;
        int kval = ( (iFE-1) % 14) / 2;
        if (iFE % 2 ==1) pData[32 + 8*irow + kval] |= ch_status & 0xFF;
        else pData[32 + 8*irow + kval] |= ((ch_status <<4) & 0xFF);

      }
    }
#endif
  }
}


void BlockFormatter::PrintSizes(FEDRawDataCollection* productRawData) {


  for (int id=0; id < 36 + 18; id++) {

    // if ( (! doBarrel_) && (id >= 9 && id <= 44)) continue;
    // if ( (! doEndCap_) && (id <= 8 || id >= 45)) continue;


    int FEDid = FEDNumbering::MINECALFEDID + id;
    FEDRawData& rawdata = productRawData -> FEDData(FEDid);
    if (rawdata.size() > 0)
      cout << "Size of FED id " << dec << FEDid << " is : " << dec << rawdata.size() << endl;

  }
}

int BlockFormatter::runType2dtt(int run_type) const{
  switch(run_type){
  case EcalDCCHeaderBlock::LASER_GAP:    //16
    return EcalDCCHeaderBlock::TTC_LASER;
  case EcalDCCHeaderBlock::TESTPULSE_GAP://17
    return EcalDCCHeaderBlock::TTC_TESTPULSE;
  case EcalDCCHeaderBlock::PEDESTAL_GAP: //18
    return EcalDCCHeaderBlock::TTC_PEDESTAL;
  case EcalDCCHeaderBlock::LED_GAP:      //19
  default:
    return 0;
  }
}
