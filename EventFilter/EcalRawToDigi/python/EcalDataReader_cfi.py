#
# Author: Ph Gras. CEA/IRFU - Saclay
#

import FWCore.ParameterSet.Config as cms

ecalDataReader = cms.EDProducer("EcalDataReader",
                                amplCut = cms.untracked.double(5.),
                                dump = cms.untracked.bool(False),
                                dumpAdc = cms.untracked.bool(True),
                                l1aHistory = cms.untracked.bool(False),
                                maxEvt  = cms.untracked.int32(10000),
                                profileFedId = cms.untracked.int32(0),
                                profileRuId = cms.untracked.int32(1),
                                l1aMinX = cms.untracked.int32(1),
                                l1aMaxX = cms.untracked.int32(601),
                                verbosity = cms.untracked.int32(0),
                                # Switch to enable ECAL DCC data dump in a binary format
                                writeDCC = cms.untracked.bool(False),
                                # fed_id: EE- is 601-609,  EB is 610-645,  EE- is 646-654
                                # when using 'single sm' fed corresponds to construction number
                                beg_fed_id = cms.untracked.int32(601),
                                end_fed_id = cms.untracked.int32(654),

                                # events as counted in the order they are written to file
                                first_event = cms.untracked.int32(0),

                                # events as counted in the order they are written to file
                                #last_event = cms.untracked.int32(99999),

                                # file name for ECAL DCC data binary dump: see writeDCC
                                # parameter.
                                dccBinDumpFileName = cms.untracked.string('dump.bin'),

                                # If non empty only listed events will be processed:
                                eventList = cms.untracked.vuint32(),

                                # Name of input collection
                                ecalRawDataCollection = cms.InputTag("rawDataCollector"),

                                # Name of output EB digi collection
                                ebDigiCollection = cms.string("ebDigis"),
                                eeDigiCollection = cms.string("eeDigis"),
                                ebSrFlagCollection = cms.string(""),
                                eeSrFlagCollection = cms.string(""),
                                tpgCollection = cms.string(""),
                                dccHeaderCollection = cms.string(""),
                                produceDigis = cms.bool(True),
                                produceSrfs = cms.bool(True),
                                produceTpgs = cms.bool(True),
                                produceDccHeaders = cms.bool(True)
                                )
import copy
#data dumper:
ecalDumpRaw = copy.deepcopy(ecalDataReader)
ecalDumpRaw.dump = True
ecalDumpRaw.produceDigis = False
ecalDumpRaw.produceSrfs = False
ecalDumpRaw.produceTpgs = False
ecalDumpRaw.produceDccHeaders = False

#real data reader:
ecalDigis = copy.deepcopy(ecalDataReader)

#simulated data reader:
simEcalDigis = copy.deepcopy(ecalDataReader)
simEcalDigis.ecalRawDataCollection = cms.InputTag("ecalPacker")
