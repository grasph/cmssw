#ifndef HCALSIMPLEAMPLITUDEZS_H
#define HCALSIMPLEAMPLITUDEZS_H 1

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimCalorimetry/HcalZeroSuppressionAlgos/interface/HcalZSAlgoEnergy.h"

/** \class HcalSimpleAmplitudeZS
	
$Date: 2007/09/13 15:02:50 $
$Revision: 1.2 $
\author J. Mans - Minnesota
*/
class HcalSimpleAmplitudeZS : public edm::EDProducer {
public:
  explicit HcalSimpleAmplitudeZS(const edm::ParameterSet& ps);
  virtual ~HcalSimpleAmplitudeZS();
  virtual void produce(edm::Event& e, const edm::EventSetup& c);
private:
  std::auto_ptr<HcalZSAlgoEnergy> hbhe_,ho_,hf_;
  edm::InputTag inputLabel_;
};

#endif
