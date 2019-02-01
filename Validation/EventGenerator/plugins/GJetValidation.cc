/*class GJetValidation
 *  
 *  Class to fill dqm monitor elements from existing EDM file
 *
 */
 
#include "Validation/EventGenerator/interface/GJetValidation.h"
#include "Validation/EventGenerator/interface/HepMCValidationHelper.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
//#include "TLorentzVector.h"

#include "CLHEP/Units/defs.h"
#include "CLHEP/Units/PhysicalConstants.h"

//#include "DataFormats/Math/interface/LorentzVector.h"
#include "Validation/EventGenerator/interface/DQMHelper.h"
using namespace edm;

GJetValidation::GJetValidation(const edm::ParameterSet& iPSet): 
  wmanager_(iPSet,consumesCollector()),
  hepMcCollection_(iPSet.getParameter<edm::InputTag>("hepmcCollection")),
  genPartCollection_(iPSet.getParameter<edm::InputTag>("genParticleCollection")),
  genJetCollection_(iPSet.getParameter<edm::InputTag>("genjetCollection"))
{    
  hepMcToken_ = consumes<HepMCProduct>(hepMcCollection_);  
  genJetToken_ = consumes<reco::GenJetCollection>(genJetCollection_);
  genPartToken_ = consumes<reco::GenParticleCollection>(genPartCollection_);
}

GJetValidation::~GJetValidation() {}

void GJetValidation::dqmBeginRun(const edm::Run& r, const edm::EventSetup& c) {
  c.getData( fPDGTable );
}

void GJetValidation::bookHistograms(DQMStore::IBooker &i, edm::Run const &, edm::EventSetup const &){

    ///Setting the DQM top directories
    std::string folderName = "Generator/GJet";
    folderName+=_name;
    DQMHelper dqm(&i);
    i.setCurrentFolder(folderName);

    // Number of analyzed events
    nEvt = dqm.book1dHisto("hEvtCnt", "Number of analyzed events", 1, 0., 1.,"bin","Event count");


    //Kinematics
    gamjMass = dqm.book1dHisto("hGJmass","m_(#gamna,j)", 100 ,0, 200,"m_{#gamma,j} (GeV)","Event * Jet counts");

    gamPt = dqm.book1dHisto("hYa", "Photon transverse momentum", 100, 0, 100,"p_{T}(#gamma)","Event count");
    gamY = dqm.book1dHisto("hYa", "Photon rapidity", 100, -5, 5,"y(#gamma)","Event count");
    return;
    

    nJets = dqm.book1dHisto("hNJets", "Jet multiplicity", 10, -0.5, 10, "Jet count", "Event count");

    nJets20 = dqm.book1dHisto("hNJets20", "Jet multiplicity (p_T > 20GeV, |y| < 4.7)", 10, -0.5, 10, "Jet count", "Event count");
    
    ptJ[0] = dqm.book1dHisto("hPtJ1","Leading jet pt", 200, 0., 200.,"p_{T}(j_{1})", "Event count");
    yJ[0] = dqm.book1dHisto("hYj1","Pt-leading jet y", 100, -5., 5.,"y(j_{1})", "Event count");

    ptJ[1] = dqm.book1dHisto("hPtJ2","Second leading jet pt", 200, 0., 200.,"p_{T}(j_{2})", "Event count");
    yJ[1] = dqm.book1dHisto("hYj2","Second pt-leading jet y", 100, -5., 5.,"y(j_{2})", "Event count");

    ptJ[2] = dqm.book1dHisto("hPtJ3","Third leading jet pt", 200, 0., 200.,"p_{T}(j_{3})", "Event count");
    yJ[2] = dqm.book1dHisto("hYj3","Third pt-leading jet y", 100, -5., 5.,"y(j_{3})", "Event count");

    ptJ[3] = dqm.book1dHisto("hPtJ4","Fourth leading jet pt", 200, 0., 200.,"p_{T}(j_{4})", "Event count");
    yJ[3] = dqm.book1dHisto("hYj4","Fourth pt-leading jet y", 100, -5., 5.,"y(j_{4})", "Event count");
}

void GJetValidation::analyze(const edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 

  return;
  // Gather information on the reco::GenParticle collection
  edm::Handle<reco::GenParticleCollection> genParticles;
  iEvent.getByToken(genPartToken_, genParticles );

  double weight = wmanager_.weight(iEvent);

  //std::cout << "weight: " << weight << std::endl;

  nEvt->Fill(0.5,weight);

  std::vector<const reco::GenParticle*> photons;
  
//  std::copy_if(genParticles->begin(), genParticles->end(),  //src
//	       photons.begin(), //dst
//	       [](const reco::GenParticle& p){ //filter
//		 return p.isPromptFinalState() && p.pdgId() == 22; //a prompt photon
//	       });
  for(const auto& p: *genParticles){
    if(p.isPromptFinalState() && p.pdgId() == 22) photons.push_back(&p);
  }

  //sort them in pt
  std::sort(photons.begin(), photons.end(),
	    [](const reco::GenParticle* a,
	       const reco::GenParticle*b){
	      return a->pt() > b->pt();
	    });
  
  if(photons.size() > 0){
    gamPt->Fill(photons[0]->pt(), weight);
    gamY->Fill(photons[0]->y(), weight);
  }

  // Gather information in the GenJet collection
  edm::Handle<reco::GenJetCollection> hdlGenJets;
  iEvent.getByToken(genJetToken_, hdlGenJets);
  std::vector<reco::GenJet> genJets = *hdlGenJets;
  
  std::sort(genJets.begin(), genJets.end(),
	    [](const reco::GenJet& a,
	       const reco::GenJet& b){
	      return a.pt() > b.pt();
	    });
  
  

  nJets->Fill(genJets.size(), weight);
  
  int nJets20_ = 0;
  for(const auto& j: genJets){
    if(j.pt() > 20 && fabs(j.y()) < 4.7) ++nJets20_;
  }

  nJets20->Fill(nJets20_, weight);

  int ij = 0;
  for(const auto& j: genJets){
    if(ij < 4){
      ptJ[ij]->Fill(j.pt(), weight);
      yJ[ij]->Fill(j.y(), weight);
    }
    
    if(photons.size() > 0 && j.pt() > 20. && fabs(j.y()) < 2.5){
      gamjMass->Fill((photons[0]->p4() + j.p4()).mass(), weight);
    }
    ++ij;
  }
}//analyze
