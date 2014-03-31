# The following comments couldn't be translated into the new config version:

#! /bin/env cmsRun

import FWCore.ParameterSet.Config as cms

process = cms.Process("validation")
process.load("DQMServices.Components.DQMEnvironment_cfi")

#keep the logging output to a nice level
process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.load("DQMServices.Core.DQM_cfg")

# load the full reconstraction configuration, to make sure we're getting all needed dependencies
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")

process.load("PhysicsTools.JetMCAlgos.CaloJetsMCFlavour_cfi")
process.load("RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff")
process.load("SimTracker.TrackHistory.TrackClassifier_cff")

process.load("Validation.RecoB.bTagAnalysis_cfi")
process.bTagValidation.jetMCSrc = 'AK5byValAlgo'
process.bTagValidation.allHistograms = True 
#process.bTagValidation.fastMC = True

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring()
)

process.GlobalTag.globaltag = 'START53_V27::All'

#===========SELECT VERTICES BASED ON CATEGORY==============

process.vertexSelectorDefaultTight = cms.EDFilter("VertexHistoryFilterOwn",
	recoVertices = cms.InputTag('inclusiveMergedVertices'),
	discriminator = cms.double(0.9),
	trackingParticles = cms.InputTag("mergedtruth","MergedTrackTruth"),
	debugMessage = cms.bool(True),
	vertexClassifierWeight = cms.PSet(
		trackingParticleSelector = cms.PSet(
        		lipTP = cms.double(30.0),
        		chargedOnlyTP = cms.bool(True),
        		pdgIdTP = cms.vint32(),
        		signalOnlyTP = cms.bool(True),
        		stableOnlyTP = cms.bool(True),
        		minRapidityTP = cms.double(-2.4),
        		minHitTP = cms.int32(0),
        		ptMinTP = cms.double(0.9),
        		maxRapidityTP = cms.double(2.4),
        		tipTP = cms.double(3.5)
    		)
	)
)

process.vertexSelectorDefaultMedium = process.vertexSelectorDefaultTight.clone( discriminator = cms.double(0.5), debugMessage = cms.bool(False) )

process.vertexSelectorDefaultLoose = process.vertexSelectorDefaultTight.clone( discriminator = cms.double(0.1), debugMessage = cms.bool(False) )

#process.vertexSelectorEditTight = process.vertexSelectorDefaultTight.clone( recoVertices = cms.InputTag('inclusiveMergedVerticesEdit') )

#process.vertexSelectorEditMedium = process.vertexSelectorEditTight.clone( discriminator = cms.double(0.5), debugMessage = cms.bool(False) )

#process.vertexSelectorEditLoose = process.vertexSelectorEditTight.clone( discriminator = cms.double(0.1), debugMessage = cms.bool(False) )


#===========FILTERS==============

process.allBParticles  = cms.EDFilter("GenParticleSelector",
     filter = cms.bool(False),
     src = cms.InputTag("genParticles"),
    cut = cms.string('((abs(pdgId) == 5) || (abs(pdgId) > 500  && abs(pdgId) < 600) || (abs(pdgId) > 5000  && abs(pdgId) < 6000))'),
     stableOnly = cms.bool(False)
)
process.bParticlesFilter  = cms.EDFilter("GenParticleSelector",
     filter = cms.bool(True),
     src = cms.InputTag("genParticles"),
    cut = cms.string('((abs(pdgId) == 5) || (abs(pdgId) > 500  && abs(pdgId) < 600) || (abs(pdgId) > 5000  && abs(pdgId) < 6000)) && pt > 100'),
     stableOnly = cms.bool(False)
)
process.cParticlesFilter  = cms.EDFilter("GenParticleSelector",
     filter = cms.bool(False),
     src = cms.InputTag("genParticles"),
    cut = cms.string('(abs(pdgId) == 4) || (abs(pdgId) > 400  && abs(pdgId) < 500) || (abs(pdgId) > 4000  && abs(pdgId) < 5000)'),
     stableOnly = cms.bool(False)
)

process.btagNonBFilter = cms.EDFilter("BTagAndFlavourFilter",
     minDiscr = cms.double(0.70),
     minPt = cms.double(30),
     jetMCSrc =  cms.InputTag('AK5byValAlgo'),
     jetTag  = cms.InputTag("combinedInclusiveSecondaryVertexBJetTags"),
     isSignal = cms.bool(False)
)


#process.bTracksProducer = cms.EDProducer("BTracksProducerOwn",
    #trackConfig = cms.PSet(process.trackClassifier),
    #simG4 = cms.InputTag("g4SimHits"),
    #trackingTruth = cms.untracked.InputTag("mergedtruth","MergedTrackTruth"),
    #trackInputTag = cms.untracked.InputTag("generalTracks"),
    #allSim = cms.untracked.bool(False)
#)
#process.bTracksProducer.trackConfig.trackProducer = cms.untracked.InputTag("generalTracks")
#process.bTracksProducer.trackConfig.enableSimToReco = cms.untracked.bool(True)
#process.bTracksProducer.trackConfig.bestMatchByMaxValue = cms.untracked.bool(True)
#process.bTracksProducer.trackingTruth.signalOnlyTP  = cms.untracked.bool(True)

#process.bTracksProducerSV0 = cms.EDProducer("BTracksProducer",
    #trackConfig = cms.PSet(process.trackClassifier),
    #simG4 = cms.InputTag("g4SimHits"),
    #trackingTruth = cms.untracked.InputTag("mergedtruth","MergedTrackTruth"),
    #trackInputTag = cms.untracked.InputTag("vertexSplitter","vertex0"),
    #allSim = cms.untracked.bool(True)
#)
#process.bTracksProducerSV0.trackConfig.enableSimToReco = cms.untracked.bool(True)
#process.bTracksProducerSV0.trackConfig.trackProducer = cms.untracked.InputTag("generalTracks")
#process.bTracksProducerSV1 = process.bTracksProducerSV0.clone(trackInputTag = cms.untracked.InputTag("vertexSplitter","vertex1"))
#process.bTracksProducerSV2 = process.bTracksProducerSV0.clone(trackInputTag = cms.untracked.InputTag("vertexSplitter","vertex2"))
#process.bTracksProducerSV3 = process.bTracksProducerSV0.clone(trackInputTag = cms.untracked.InputTag("vertexSplitter","vertex3"))
#process.bTracksProducerSV4 = process.bTracksProducerSV0.clone(trackInputTag = cms.untracked.InputTag("vertexSplitter","vertex4"))

#process.bTracksProducerSV0.trackConfig.trackProducer = cms.untracked.InputTag("vertexSplitter","vertex0")
#process.bTracksProducerSV1.trackConfig.trackProducer = cms.untracked.InputTag("vertexSplitter","vertex1")
#process.bTracksProducerSV2.trackConfig.trackProducer = cms.untracked.InputTag("vertexSplitter","vertex2")
#process.bTracksProducerSV3.trackConfig.trackProducer = cms.untracked.InputTag("vertexSplitter","vertex3")
#process.bTracksProducerSV4.trackConfig.trackProducer = cms.untracked.InputTag("vertexSplitter","vertex4")



#==========SELECT 5 VERTICES===================

process.vertexSplitter = cms.EDProducer("VertexTrackSplitter",
    vertexInputTag = cms.InputTag("inclusiveMergedVertices"),
    max = cms.untracked.uint32(5)
)


#==========CREATE TRACK COLLECTIONS FOR EACH OF THESE FIVE VERTICES===================

process.vertexSelectorDefaultMediumSV0 = process.vertexSelectorDefaultMedium.clone( recoVertices = cms.InputTag('vertexSplitter', 'vertex0') )

process.vertexSelectorDefaultMediumSV1 = process.vertexSelectorDefaultMedium.clone( recoVertices = cms.InputTag('vertexSplitter', 'vertex1') )

process.vertexSelectorDefaultMediumSV2 = process.vertexSelectorDefaultMedium.clone( recoVertices = cms.InputTag('vertexSplitter', 'vertex2') )

process.vertexSelectorDefaultMediumSV3 = process.vertexSelectorDefaultMedium.clone( recoVertices = cms.InputTag('vertexSplitter', 'vertex3') )

process.vertexSelectorDefaultMediumSV4 = process.vertexSelectorDefaultMedium.clone( recoVertices = cms.InputTag('vertexSplitter', 'vertex4') )


#process.p = cms.Path(process.allBParticles * process.bParticlesFilter* process.cParticlesFilter* process.bTracksProducer * process.myPartons* process.AK5Flavour * process.btagging * process.inclusiveVertexing * process.inclusiveSecondaryVertexFinderTagInfos * process.combinedInclusiveSecondaryVertexBJetTags)
#process.p = cms.Path(process.allBParticles * process.bParticlesFilter * process.cParticlesFilter * process.myPartons* process.AK5Flavour * process.btagging * process.inclusiveVertexing * process.inclusiveSecondaryVertexFinderTagInfos * process.combinedInclusiveSecondaryVertexBJetTags *  process.bTracksProducer * process.vertexSplitter * process.bTracksProducerSV0 * process.bTracksProducerSV1 * process.bTracksProducerSV2 * process.bTracksProducerSV3 * process.bTracksProducerSV4 )



#==========WRITE THE PATHS===================

process.pall = cms.Path(process.myPartons * process.AK5Flavour * process.btagging * process.inclusiveVertexing * process.inclusiveSecondaryVertexFinderTagInfos * process.combinedInclusiveSecondaryVertexBJetTags * process.vertexSelectorDefaultTight * process.vertexSelectorDefaultMedium * process.vertexSelectorDefaultLoose * process.vertexSplitter * process.vertexSelectorDefaultMediumSV0 * process.vertexSelectorDefaultMediumSV1 * process.vertexSelectorDefaultMediumSV2 * process.vertexSelectorDefaultMediumSV3 * process.vertexSelectorDefaultMediumSV4 )

process.pfake = cms.Path(process.myPartons * process.AK5Flavour * process.btagging * process.inclusiveVertexing * process.inclusiveSecondaryVertexFinderTagInfos * process.combinedInclusiveSecondaryVertexBJetTags * process.btagNonBFilter * process.vertexSelectorDefaultTight * process.vertexSelectorDefaultMedium * process.vertexSelectorDefaultLoose * process.vertexSplitter * process.vertexSelectorDefaultMediumSV0 * process.vertexSelectorDefaultMediumSV1 * process.vertexSelectorDefaultMediumSV2 * process.vertexSelectorDefaultMediumSV3 * process.vertexSelectorDefaultMediumSV4 )





process.PoolSource.fileNames = [
  #"file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_5_3_14/src/Validation/RecoB/input_rootfiles/ttjets_8tev_mad_recodebug.root"
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/000E24FB-5025-E211-91E3-0030487D5E9D.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/002A1E74-5D25-E211-843E-003048C693F8.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/007147F7-FA25-E211-ABE9-00266CFFB390.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/007686A5-5425-E211-A02A-00266CFFA780.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00B0D68D-5A25-E211-BA94-002481E0D398.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00B426A2-5625-E211-BED6-0025901D4B4C.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00BCB78E-5825-E211-B7F3-003048D436D2.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00C98A01-5B25-E211-BE79-00215AEDFD12.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00D1D3CC-7A25-E211-A0E9-0025904B141E.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00DD5739-6025-E211-A623-0030487E4B8F.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00DE7CF3-E825-E211-9610-00266CFFA23C.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00E123DC-4725-E211-82E5-00266CFB8D74.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00E93433-4125-E211-A866-00266CF1074C.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02153AC3-5525-E211-AA53-003048D436D2.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02162455-E225-E211-9409-003048D4DFBC.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02354B69-5B25-E211-8744-003048D43838.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02472875-5625-E211-9013-003048C692FA.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/025C740B-5A25-E211-B824-003048F0E82C.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02AD950E-6C25-E211-B2B7-0025901D4C92.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02B9D84C-5725-E211-B4BC-0025904B1446.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02D53154-6C25-E211-AC9D-003048F0E526.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/042DD7BE-4325-E211-AC97-00266CF32F18.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/047DE5F2-6A25-E211-9861-0025904B12DE.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0490531E-5A25-E211-A09A-00215AD4D6E2.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/04A0847D-5A25-E211-A95C-003048D47912.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/04C76AF6-6025-E211-AB72-00266CF33340.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/04E30D2B-6D25-E211-B1A1-003048D43726.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/04EC2A74-6C25-E211-B1C4-00266CFFA5CC.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/04F90F0E-4025-E211-A7EC-0030487D5EA7.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/060CBF34-5F25-E211-BCA7-0025901D4C18.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0682AEFF-5B25-E211-876A-00266CFFA344.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0693950B-6C25-E211-A37A-0025901D4B08.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/069759ED-EE25-E211-9176-0025901D4B20.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/06D1B831-4925-E211-B90D-0025904B1444.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/06D5BF80-6225-E211-94A1-00266CF9C1AC.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/085199F9-6925-E211-8A67-00266CF327C4.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0856436A-4025-E211-8B36-003048C692DA.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0873919E-5025-E211-B17D-002590494CB2.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/08A5F2D8-4625-E211-9141-003048D3C7DC.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/08D30598-5F25-E211-A5E9-003048F0E1B2.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/08DD1CD9-4325-E211-9E03-0025904B12DE.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0A21892F-5425-E211-BB74-002481E94B26.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0A4069CA-5F25-E211-A4E8-003048C692E0.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0A45034B-4025-E211-9CBF-0030487F1F2F.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0A79ACD3-6B25-E211-AD1A-003048F0E822.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0A7F7D9B-5925-E211-B5D4-003048D4DFAA.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0A8CB1DF-5B25-E211-9C20-0025901D4AF0.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0AB2C3D9-5B25-E211-BAE1-003048D4397E.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0AB9E748-4425-E211-820C-00266CFFB1F4.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0C28F18B-F425-E211-8D5B-0030487F91DB.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0C2F71CC-5A25-E211-8EC5-002481E0D2E8.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0C3CAD7A-6D25-E211-8102-0030487D7103.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0C58EF24-5625-E211-9B9A-003048D47912.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0C5AFABC-6025-E211-9ADE-0025901D4844.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0C661F5B-5A25-E211-8BD6-003048F0EBB8.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0CF054DC-5F25-E211-B6B2-003048C66184.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0E3EE564-5625-E211-8E37-0030487D5EA7.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0E956A44-3625-E211-AF15-002590494CE4.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0EC6F882-5B25-E211-A192-00266CF2AE10.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0EC86B4A-6025-E211-8374-00266CF3336C.root",
  "/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0ED174D2-5C25-E211-A30B-0030487D5DBD.root"
#"file:/networkdata/arizzi/TTBARRECODEBUG/0AB9E748-4425-E211-820C-00266CFFB1F4.root",
#"file:/networkdata/arizzi/TTBARRECODEBUG/0C28F18B-F425-E211-8D5B-0030487F91DB.root",
#"file:/networkdata/arizzi/TTBARRECODEBUG/0C2F71CC-5A25-E211-8EC5-002481E0D2E8.root",
#"file:/networkdata/arizzi/TTBARRECODEBUG/0C3CAD7A-6D25-E211-8102-0030487D7103.root",
#"file:/networkdata/arizzi/TTBARRECODEBUG/0C58EF24-5625-E211-9B9A-003048D47912.root",
#"file:/networkdata/arizzi/TTBARRECODEBUG/0C5AFABC-6025-E211-9ADE-0025901D4844.root",
#"file:/networkdata/arizzi/TTBARRECODEBUG/0C661F5B-5A25-E211-8BD6-003048F0EBB8.root",
#"file:/networkdata/arizzi/TTBARRECODEBUG/0CF054DC-5F25-E211-B6B2-003048C66184.root",
#"file:/networkdata/arizzi/TTBARRECODEBUG/0E3EE564-5625-E211-8E37-0030487D5EA7.root"
#]
#aa=[
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/00020B8A-DCF3-E111-B63E-00266CF33340.root",
##"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/0003943E-DFF3-E111-A054-0025904B1446.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/0055C993-E4F3-E111-9674-003048F0E184.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/00EBF575-E0F3-E111-A6C7-0030487D5E9D.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/0212A580-DFF3-E111-8389-003048D4DFA6.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/021AA457-E3F3-E111-930E-0025901D4C46.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/02730427-E2F3-E111-A21E-00266CFFA2B8.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/02CF08B4-E0F3-E111-9AF4-00266CF2AE10.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/02E06584-E3F3-E111-8BFF-002481E0D714.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/046C5A9C-E4F3-E111-B2AC-00266CFFA23C.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/0471AE09-E2F3-E111-99D3-00266CFF9FFC.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/047F17FC-DCF3-E111-9E4E-0025901D4D54.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/048739FC-E4F3-E111-B79C-0025901D4C3C.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/04E7C172-E0F3-E111-B302-0025904B12E0.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/04F6AA7F-E0F3-E111-A45F-002481E94BFE.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/FEF5C099-DEF3-E111-AFE3-00266CF32E70.root",
##"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/1E903A12-E7F3-E111-8EC4-003048D43958.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/E680E1B5-E3F3-E111-BC5C-00215AD4D670.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/1E9A16ED-DFF3-E111-AB70-0030487F933D.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/1CC6AAD3-E0F3-E111-89A5-0030487E4B8D.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/E63A622D-40F4-E111-94BA-0025904B578E.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/1CC703BF-E1F3-E111-BFA0-002481E0D144.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/E618D7B3-E4F3-E111-86C6-003048C693D6.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/1CE1A35A-E1F3-E111-B695-0025901D4090.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/E4F0D3E5-DFF3-E111-ACB5-003048C69310.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/1E8C90AF-DEF3-E111-9633-00266CFFA2D0.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/E4DD182D-DFF3-E111-ADF4-0025901D4C3C.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/1EB621B8-DEF3-E111-B521-003048C69310.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/1EFCFA15-E4F3-E111-8D9A-0030487E5179.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/1EBB8017-DFF3-E111-93C7-0025901D4C18.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/200C3B02-E2F3-E111-A0C5-0025904B1446.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/200E4D0A-DFF3-E111-A932-0025904B5FBA.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/201D1ED7-40F4-E111-AB62-003048D4DF90.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/20281D46-E1F3-E111-99A9-0025904B12B2.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/1ED84632-E1F3-E111-A1BB-003048C68A9E.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/2061FF3B-E4F3-E111-8F86-003048F0E18E.root",
#"file:/networkdata/cvernier/validation_533/CMSSW_5_3_3_patch2/src/Validation/RecoB/1EEC3299-E3F3-E111-AF4B-00266CF32A20.root"
#file:/networkdata/arizzi/QCD470600/reco/001793DE-DB07-E211-B4CE-003048679162.root'
]

process.outfake = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('reco_bDebugOwn_2_fake_100.root'),
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring("pfake"))
)    

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('reco_bDebugOwn_2_all_100.root'),
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring("pall"))
)
process.endpath= cms.EndPath(process.outfake * process.out)


process.load('CondCore.DBCommon.CondDBSetup_cfi')
process.BTauMVAJetTagComputerRecord = cms.ESSource('PoolDBESSource',
    process.CondDBSetup,
    timetype = cms.string('runnumber'),
    toGet = cms.VPSet(cms.PSet(
        record = cms.string('BTauGenericMVAJetTagComputerRcd'),
        tag = cms.string('MVAComputerContainer_Retrained53X_JetTags_v2')
    )),
    connect = cms.string('frontier://FrontierProd/CMS_COND_PAT_000'),
    BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
)
process.es_prefer_BTauMVAJetTagComputerRecord = cms.ESPrefer('PoolDBESSource','BTauMVAJetTagComputerRecord')


