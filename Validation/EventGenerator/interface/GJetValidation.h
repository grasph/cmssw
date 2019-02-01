#ifndef GJETVALIDATION_H
#define GJETVALIDATION_H

/*class DrellYanValidation
 *  
 *  Class to fill Event Generator dqm monitor elements; works on HepMCProduct
 *
 *
 */

// framework & common header files
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Run.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

//DQM services
#include "DQMServices/Core/interface/DQMStore.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
#include "Validation/EventGenerator/interface/WeightManager.h"

#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

class GJetValidation: public DQMEDAnalyzer{

public:
  explicit GJetValidation(const edm::ParameterSet&);
  ~GJetValidation() override;

  void bookHistograms(DQMStore::IBooker &i, edm::Run const &, edm::EventSetup const &) override;
  void dqmBeginRun(const edm::Run& r, const edm::EventSetup& c) override;
  void analyze(edm::Event const&, edm::EventSetup const&) override;

private:
  WeightManager wmanager_;
  edm::InputTag hepMcCollection_;
  edm::InputTag genPartCollection_;
  edm::InputTag genJetCollection_;

  /// PDT table
  edm::ESHandle<HepPDT::ParticleDataTable> fPDGTable ;

  // Number of analyzed events
  MonitorElement*  nEvt;
    
  //Kinematics
  MonitorElement* gamjMass;

  MonitorElement* gamPt;
  MonitorElement* gamY;
  MonitorElement* nJets;
  MonitorElement* nJets20;
  MonitorElement* ptJ[4];
  MonitorElement* yJ[4];
  
  /// decay flavor
  int _flavor;
  /// decay flavor name
  std::string _name;

  edm::EDGetTokenT<edm::HepMCProduct> hepMcToken_;
  edm::EDGetTokenT<reco::GenJetCollection> genJetToken_;
  edm::EDGetTokenT<reco::GenParticleCollection> genPartToken_;

};

#endif //GJETVALIDATION_H not defined
