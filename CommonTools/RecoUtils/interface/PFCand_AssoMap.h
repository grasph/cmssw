#ifndef PFCand_AssoMap_h
#define PFCand_AssoMap_h

// -*- C++ -*-
//
// Package:    PFCand_AssoMap
// Class:      PFCand_AssoMap
// 
/**\class PFCand_AssoMap PFCand_AssoMap.cc CommonTools/RecoUtils/plugins/PFCand_AssoMap.cc

  Description: Produces a map with association between pf candidates and their particular most probable vertex with a quality of this association
*/
//
// Original Author:  Matthias Geisler
//         Created:  Wed Apr 18 14:48:37 CEST 2012
// $Id: PFCand_AssoMap.h,v 1.6 2012/10/10 11:43:31 mgeisler Exp $
//
//

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "CommonTools/RecoUtils/interface/PF_PU_AssoMapAlgos.h"
#include "CommonTools/RecoUtils/interface/PFCand_NoPU_WithAM_Algos.h"


//
// class declaration
//

class PFCand_AssoMap : public edm::EDProducer, private PF_PU_AssoMapAlgos {
   public:
      explicit PFCand_AssoMap(const edm::ParameterSet&);
      ~PFCand_AssoMap();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void produce(edm::Event&, const edm::EventSetup&);

      // ----------member data ---------------------------

      edm::InputTag input_PFCandidates_;
};


#endif
