## import skeleton process
from PhysicsTools.PatAlgos.patTemplate_cfg import *

## ------------------------------------------------------
#  switch to CaloTau. Outputs will be:
#  - patCaloTaus,
#  - selectedPatCaloTaus,
#  - cleanPatCaloTaus
## ------------------------------------------------------
from PhysicsTools.PatAlgos.tools.tauTools import switchToCaloTau
switchToCaloTau(process)

## let it run
process.p = cms.Path(
            process.patDefaultSequence
            )

## ------------------------------------------------------
#  In addition you usually want to change the following
#  parameters:
## ------------------------------------------------------
#
#   process.GlobalTag.globaltag =  ...    ##  (according to https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideFrontierConditions)
#                                         ##
#   process.source.fileNames =  ...       ##  (e.g. 'file:AOD.root')
#                                         ##
process.maxEvents.input = 10
#                                         ##
#   process.out.outputCommands = [ ... ]  ##  (e.g. taken from PhysicsTools/PatAlgos/python/patEventContent_cff.py)
#                                         ##
process.out.fileName = 'patTuple_caloTaus.root'
#                                         ##
#   process.options.wantSummary = False   ##  (to suppress the long output at the end of the job)
