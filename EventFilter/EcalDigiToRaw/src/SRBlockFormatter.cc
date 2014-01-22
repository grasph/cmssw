//Emacs settings:-*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil -*-

#include <memory>

#include "EventFilter/EcalDigiToRaw/interface/SRBlockFormatter.h"
#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"
#include "EventFilter/EcalDigiToRaw/interface/EcalDigiToRaw.h"

using namespace std;

//Number of channels of EE DCCs
//DCC ID:                    1   2   3   4   5   6   7   8   9
static int nEeDccChannels[9] = {34, 32, 33, 33, 32, 34, 33, 34, 33};

//(DCC id -1) -> SRP ID
static int srpids[54] = {
  1,  17, 33,  2, 18, 34,  3, 19, 35, 20, 36, 52, 68, 84,  5, 21, 37, 53,
  69, 85,  6, 22, 38, 54, 70, 86,  4, 23, 39, 55, 71, 87,  8, 24, 40, 56,
  72, 88,  9, 25, 41, 57, 73, 89,  7, 10, 26, 42, 11, 27, 43, 12, 28, 44
};

SRBlockFormatter::SRBlockFormatter() {

}

SRBlockFormatter::~SRBlockFormatter() {

}


void SRBlockFormatter::StartEvent() {
  if (debug_) cout << "enter in StartEvent. header_ size is  " << header_.size() << endl;
  header_ .clear() ;
  if (debug_) cout << "after empty : header_ size is  " << header_.size() << endl;
  return;
}

void SRBlockFormatter::DigiToRaw(int dccid, int dcc_channel, int flag, FEDRawData& rawdata)
{

  if (debug_) cout << "enter in SRBlockFormatter::DigiToRaw " << endl;
  if (debug_) print(rawdata);

  //  int bx = *pbx_;
  //int lv1 = *plv1_;
  int bx = 0;
  int lv1 = 0;
  EcalDCCHeaderBlock dccHeader = base_->GetDCCHeader(dccid);
  bx = dccHeader.getSRPBx();
  lv1 = dccHeader.getSRPLv1();
  int Nrows_SRP = 5;   // Both for Barrel and EndCap (without the header row)
  int SRid = (dccid - 1) / 3 +1;


  // int Number_SRP_Flags = 68;

  int SRP_index;
  int icode = 1000 * dccid +  SRid;
  if (debug_) cout << "size of header_ map is " << header_.size() << endl;

  std::map<int, int>::const_iterator it_header = header_.find(icode);

  if ( it_header != header_.end() ) {
    SRP_index = rawdata.size() / 8 - Nrows_SRP;
    if (debug_) cout << "This SRid is already there." << endl;
  }
  else {
    if (debug_) cout << "New SR Block added on Raw data " << endl;
    header_[icode] = 1;
    SRP_index = rawdata.size() / 8;
    rawdata.resize (rawdata.size() + 8 + 8 * Nrows_SRP);  // 1 line for SRP header, 5 lines of data
    unsigned char* ppData = rawdata.data();
    //    ppData[8*SRP_index] = SRid & 0xFF;
    if(unsigned(dccid) < 55) ppData[8*SRP_index] = srpids[dccid - 1];
    ppData[8*SRP_index+2] = bx & 0xFF;
    ppData[8*SRP_index+3] = (bx & 0xF00)>>8;
    ppData[8*SRP_index+3] |= 0x80;
    ppData[8*SRP_index+4] = lv1 & 0xFF;
    ppData[8*SRP_index+5] = (lv1 & 0xF00)>>8;
    int nchs = 68;
    if((dccid - 1) % 45 < 9){ //endcap
      nchs = nEeDccChannels[(dccid - 1) % 45];
    }
    ppData[8*SRP_index+6] = nchs;
    ppData[8*SRP_index+7] = 0x80;
    SRP_index ++;
    if (debug_) cout << "Added headers and empty lines : " << endl;
    if (debug_) print(rawdata);

    // -- put the B011 and B100 already, since for Endcap there can be empty
    // -- lines in the TCC and the SRP blocks
    unsigned char* Data = rawdata.data();
    for (int iline=SRP_index; iline < SRP_index + Nrows_SRP; iline++) {
      Data[8*iline + 7] |= 0x80;
      Data[8*iline + 3] |= 0x80;
    }
  }

  unsigned char* pData = rawdata.data();

  // -- Now the TCC Block :

  //  //increment the number of SR flags in the data header:
  //  int nflags = pData[8*(SRP_index-1) +6] & 0x7F;
  //  nflags ++;
  //  pData[8*(SRP_index-1) + 6] = nflags & 0x7F;

  int jTT = (dcc_channel-1);
  int irow = jTT/16 ;
  int ival = jTT % 4;
  int kval = (jTT % 16) / 4;
  SRP_index += irow;

  if (debug_) cout << "Now add SC to SRBlock " << dec << dcc_channel << " irow ival " << dec << irow << " " << dec << ival << endl;
  if (debug_) cout << "new data will be added at line " << dec << SRP_index << endl;


  unsigned char* buff = &pData[8*SRP_index];
  Word64* pw = reinterpret_cast<Word64*>(const_cast<unsigned char*>(buff));
  int nbits = kval*16 + 3*ival;
  Word64 wflag = (Word64)((short int)flag & 0x7) << nbits;
  *pw |= wflag;
  Word64 b1 = (Word64)((Word64)0x80 <<56);
  *pw |= b1 ;
  Word64 b2 = (Word64)((Word64)0x80 <<24);
  *pw |= b2 ;

  if (debug_) print(rawdata);
}
