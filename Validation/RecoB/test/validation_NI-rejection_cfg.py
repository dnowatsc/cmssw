#Basic example configration file to run the b-tagging validation sequence.
import FWCore.ParameterSet.Config as cms
process = cms.Process("validation")

"""
start customization
"""

#Enter here the Global tags
tag =  'POSTLS170_V5::All'
#Flavour plots for MC: "all" = plots for all jets ; "dusg" = plots for d, u, s, dus, g independently ; not mandatory and any combinations are possible 
#b, c, light (dusg), non-identified (NI), PU jets plots are always produced
flavPlots = "allbcldusg"
#Check if jets originate from PU? option recommended (only for MC)
PUid = True
#List of taggers and taginfo to be considered (see example in: DQMOffline/RecoB/python/bTagCommon_cff.py)
from DQMOffline.RecoB.bTagCommon_cff import *
tagConfig = cms.VPSet(
        cms.PSet(
            bTagGenericAnalysisBlock,
            label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTagsNoMin2D"),
            folder = cms.string("CSVIVFv2-NoMin2D")
        ),
		cms.PSet(
            bTagGenericAnalysisBlock,
            label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned"),
            folder = cms.string("CSVIVFv2-NICleaned")
        ),
)

"""
end customization
"""

###prints###
print "Global Tag : ", tag
############

process.load("DQMServices.Components.DQMEnvironment_cfi")
process.load("DQMServices.Core.DQM_cfg")

#keep the logging output to a nice level
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

#for MC jet flavour
process.load("PhysicsTools.JetMCAlgos.CaloJetsMCFlavour_cfi")
process.AK4byRef.jets = cms.InputTag("ak4PFJetsCHS")
process.flavourSeq = cms.Sequence(
    process.myPartons *
    process.AK4Flavour
)

#=============
# read in different calibration records

process.load('CondCore.DBCommon.CondDBSetup_cfi')
process.BTauMVAJetTagComputerRecord = cms.ESSource('PoolDBESSource',
    process.CondDBSetup,
    timetype = cms.string('runnumber'),
    toGet = cms.VPSet(cms.PSet(
        record = cms.string('BTauGenericMVAJetTagComputerRcd'),
        tag = cms.string('MVAComputerContainer_53X_JetTags_v3')
    )),
    connect = cms.string('frontier://FrontierProd/CMS_COND_PAT_000'),
    BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
)
process.es_prefer_BTauMVAJetTagComputerRecord = cms.ESPrefer('PoolDBESSource','BTauMVAJetTagComputerRecord')

#=============

#Validation sequence
process.load("Validation.RecoB.bTagAnalysis_cfi")
process.bTagValidation.jetMCSrc = 'AK4byValAlgo'
process.bTagValidation.tagConfig = tagConfig
process.bTagHarvestMC.tagConfig = tagConfig
process.bTagValidation.flavPlots = flavPlots
process.bTagHarvestMC.flavPlots = flavPlots
process.bTagValidation.doPUid = cms.bool(PUid)
process.ak4GenJetsForPUid = cms.EDFilter("GenJetSelector",
                                         src = cms.InputTag("ak4GenJets"),
                                         cut = cms.string('pt > 8.'),
                                         filter = cms.bool(False)
                                         )
process.load("PhysicsTools.PatAlgos.mcMatchLayer0.jetMatch_cfi")
process.patJetGenJetMatch.matched = cms.InputTag("ak4GenJetsForPUid")
process.patJetGenJetMatch.maxDeltaR = cms.double(0.25)
process.patJetGenJetMatch.resolveAmbiguities = cms.bool(True)

# load the full reconstraction configuration, to make sure we're getting all needed dependencies
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.Geometry.GeometryIdeal_cff")

process.GlobalTag.globaltag = tag

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring()
)

# load b-tagging sequences including NI-rejection

process.load("Validation.RecoB.bTagNIRejSeq_cff")

process.bTagSeq = cms.Sequence(
	process.MYbtagSequenceNoMin2D *
	process.MYbtagSequenceNIremovedWithCuts
)

process.dqmSeq = cms.Sequence(process.ak4GenJetsForPUid * process.patJetGenJetMatch * process.flavourSeq * process.bTagValidation * process.bTagHarvestMC * process.dqmSaver)

process.plots = cms.Path(process.bTagSeq * process.dqmSeq)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(301)
)
    
process.dqmEnv.subSystemFolder = 'BTAG'
process.dqmSaver.producer = 'DQM'
process.dqmSaver.workflow = '/POG/BTAG/BJET'
process.dqmSaver.convention = 'Offline'
process.dqmSaver.saveByRun = cms.untracked.int32(-1)
process.dqmSaver.saveAtJobEnd =cms.untracked.bool(True) 
process.dqmSaver.forceRunNumber = cms.untracked.int32(1)
process.PoolSource.fileNames = [
	'/store/relval/CMSSW_7_2_0_pre1/RelValTTbar_13/GEN-SIM-RECO/POSTLS172_V1-v1/00000/0C41D097-EEFD-E311-9E85-0025905A6066.root',
	'/store/relval/CMSSW_7_2_0_pre1/RelValTTbar_13/GEN-SIM-RECO/POSTLS172_V1-v1/00000/26C5E596-F0FD-E311-A15D-0025905B8596.root',
	'/store/relval/CMSSW_7_2_0_pre1/RelValTTbar_13/GEN-SIM-RECO/POSTLS172_V1-v1/00000/8C29EAA9-F0FD-E311-B63D-003048FFD736.root'
]

