import FWCore.ParameterSet.Config as cms

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
gjetValidation = DQMEDAnalyzer('GJetValidation',
                               hepmcCollection = cms.InputTag("generatorSmeared"),
                               genParticleCollection = cms.InputTag("genParticles"),
                               genjetCollection = cms.InputTag("ak4GenJets"),
                               UseWeightFromHepMC = cms.bool(True)
                               )
