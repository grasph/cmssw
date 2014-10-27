//Emacs settings:-*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil -*-

#ifndef BLOCKFORMATTER_H
#define BLOCKFORMATTER_H

#include <iostream>
#include <vector>
#include <map>

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDRawData.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/EcalRawData/interface/EcalDCCHeaderBlock.h"
#include "DataFormats/EcalRawData/interface/EcalDCCHeaderBlock.h"
#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"

class EcalDigiToRaw;


class BlockFormatter {
public :
  typedef uint64_t Word64;
  typedef uint16_t Word16;

  BlockFormatter();
  ~BlockFormatter();
  void SetParam(EcalDigiToRaw* base);
  static const int kCardsPerTower = 5;     // Number of VFE cards per trigger tower
  void DigiToRaw(FEDRawDataCollection* productRawData);
  void print(FEDRawData& rawdata);
  // void CleanUp(FEDRawDataCollection* productRawData);
  void CleanUp(FEDRawDataCollection* productRawData,
               std::map<int, std::map<int,int> >* FEDorder);
  void PrintSizes(FEDRawDataCollection* productRawData);

protected:
  /** Converts EcalDCCHeaderBlock::EcalDCCRuntype to Detailed Trigger Type
   * for in-abort-gap calibration.
   */
  int runType2dtt(int run_type) const;

protected :
  bool debug_;

  bool doBarrel_;
  bool doEndCap_;
  bool doTCC_;
  bool doSR_;
  bool doTower_;

  std::vector<int32_t> * plistDCCId_;

  int* pcounter_;
  int* porbit_number_;
  int* pbx_;
  int* plv1_;
  int* prunnumber_;
  EcalDigiToRaw* base_;
};
#endif
