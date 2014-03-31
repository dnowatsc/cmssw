# The following comments couldn't be translated into the new config version:

#! /bin/env cmsRun

import FWCore.ParameterSet.Config as cms

process = cms.Process("validation")
#process.load("DQMServices.Components.DQMEnvironment_cfi")

#keep the logging output to a nice level
process.load("FWCore.MessageLogger.MessageLogger_cfi")

#process.load("DQMServices.Core.DQM_cfg")

# load the full reconstraction configuration, to make sure we're getting all needed dependencies
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")

#process.load("SimTracker.TrackHistory.Playback_cff")
process.load("SimTracker.TrackHistory.VertexClassifier_cff")
process.load("RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff")
process.load("SimTracker.VertexAssociation.VertexAssociatorByTracks_cfi")

process.VertexAssociatorByTracksESProducer.trackingParticleSelector.stableOnlyTP = cms.bool(True)

#from SimTracker.TrackHistory.CategorySelectors_cff import *

#process.load("PhysicsTools.JetMCAlgos.CaloJetsMCFlavour_cfi")  

#process.load("Validation.RecoB.bTagAnalysis_cfi")
#process.bTagValidation.jetMCSrc = 'AK5byValAlgo'
#process.bTagValidation.allHistograms = True 
#process.bTagValidation.fastMC = True

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(__MAX_EVENTS__) #__MAX_EVENTS__
)
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(__FILE_NAMES__), #__FILE_NAMES__
    skipEvents = cms.untracked.uint32(__SKIP_EVENTS__)
)

process.GlobalTag.globaltag = 'START53_V27::All'

#process.allBParticles  = cms.EDFilter("GenParticleSelector",
     #filter = cms.bool(False),
     #src = cms.InputTag("genParticles"),
    #cut = cms.string('((abs(pdgId) == 5) || (abs(pdgId) > 500  && abs(pdgId) < 600) || (abs(pdgId) > 5000  && abs(pdgId) < 6000))'),
     #stableOnly = cms.bool(False)
#)
#process.bParticlesFilter  = cms.EDFilter("GenParticleSelector",
     #filter = cms.bool(True),
     #src = cms.InputTag("genParticles"),
    #cut = cms.string('((abs(pdgId) == 5) || (abs(pdgId) > 500  && abs(pdgId) < 600) || (abs(pdgId) > 5000  && abs(pdgId) < 6000)) && pt > 100'),
     #stableOnly = cms.bool(False)
#)
#process.cParticlesFilter  = cms.EDFilter("GenParticleSelector",
     #filter = cms.bool(False),
     #src = cms.InputTag("genParticles"),
    #cut = cms.string('(abs(pdgId) == 4) || (abs(pdgId) > 400  && abs(pdgId) < 500) || (abs(pdgId) > 4000  && abs(pdgId) < 5000)'),
     #stableOnly = cms.bool(False)
#)

#=============FILTER TRACKS FROM PRIMARY VERTICES====================

process.filteredTracks = cms.EDProducer("TrackPrimaryVertexFilter",
	primaryVertices = cms.InputTag("offlinePrimaryVertices"),
	tracks = cms.InputTag("generalTracks")
)

#=============RECONSTRUCT THE SECONDARY VERTICES====================

process.inclusiveVertexFinderEdit = cms.EDProducer("InclusiveVertexFinderMod",
	beamSpot = cms.InputTag("offlineBeamSpot"),
	primaryVertices = cms.InputTag("offlinePrimaryVertices"),
	tracks = cms.InputTag("filteredTracks"),
	minHits = cms.uint32(6), #default 8
	maximumLongitudinalImpactParameter = cms.double(50.0), #default 0.3
	minPt = cms.double(0.4), # default 0.8
	maxNTracks = cms.uint32(30),
	seedPointMinDist = cms.double(2.0),
	
	clusterizer = cms.PSet(
		seedMin3DIPSignificance = cms.double(1.2),
		seedMin3DIPValue = cms.double(0.005),
		clusterMaxDistance = cms.double(0.05), #500um
		clusterMaxSignificance = cms.double(4.5), #4.5 sigma
		clusterScale = cms.double(1), 
		clusterMinAngleCosine = cms.double(0.5), # only forward decays
	),

	vertexMinAngleCosine = cms.double(0.5), # scalar prod direction of tracks and flight dir
	vertexMinDLen2DSig = cms.double(2.5), #2.5 sigma
	vertexMinDLenSig = cms.double(0.5), #0.5 sigma
	vertexReco = cms.PSet(
		finder = cms.string('avr'),
		primcut = cms.double(1.0),
		seccut = cms.double(3),
		smoothing = cms.bool(True)
	)
)

process.vertexMergerEdit = process.vertexMerger.clone(
	secondaryVertices = cms.InputTag("inclusiveVertexFinderEdit")
)

process.trackVertexArbitratorEdit = process.trackVertexArbitrator.clone(
	secondaryVertices = cms.InputTag("vertexMergerEdit"),
	tracks = cms.InputTag("filteredTracks")
)

process.inclusiveMergedVerticesEdit = process.inclusiveMergedVertices.clone(
	secondaryVertices = cms.InputTag("trackVertexArbitratorEdit")
)

process.inclusiveVertexingEdit = cms.Sequence(process.inclusiveVertexFinderEdit * process.vertexMergerEdit * process.trackVertexArbitratorEdit * process.inclusiveMergedVerticesEdit)

#===========SELECT VERTICES BASED ON CATEGORY==============

process.vertexSelectorDefault = cms.EDFilter("VertexHistoryFilterOwn",
	recoVertices = cms.InputTag('inclusiveMergedVertices'),
	discriminator = cms.double(0.0),
	trackingParticles = cms.InputTag("mergedtruth","MergedTrackTruth"),
	debugMessage = cms.bool(False),
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

#process.vertexSelectorDefaultMedium = process.vertexSelectorDefaultTight.clone( discriminator = cms.double(0.5), debugMessage = cms.bool(False) )

#process.vertexSelectorDefaultLoose = process.vertexSelectorDefaultTight.clone( discriminator = cms.double(0.1), debugMessage = cms.bool(False) )

#process.vertexSelectorEditTight = process.vertexSelectorDefaultTight.clone( recoVertices = cms.InputTag('inclusiveMergedVerticesEdit') )

#process.vertexSelectorEditMedium = process.vertexSelectorEditTight.clone( discriminator = cms.double(0.5), debugMessage = cms.bool(False) )

#process.vertexSelectorEditLoose = process.vertexSelectorEditTight.clone( discriminator = cms.double(0.1), debugMessage = cms.bool(False) )

#=============REMOVE ALL NUCLEAR INTERACION VERTICES USING VERTEXCLEANER====================

process.hadronicCleanedDefLoose = cms.EDProducer("VertexCleaner",
	primaryVertices= cms.InputTag("vertexSelectorDefault", "HadronicProcess"),
	secondaryVertices = cms.InputTag("inclusiveMergedVertices"),
	maxFraction = cms.double(0.0)
)

#process.hadronicCleanedDefMedium = process.hadronicCleanedDefLoose.clone(primaryVertices = cms.InputTag("vertexSelectorDefaultMedium", "HadronicProcess"))

#process.hadronicCleanedDefTight = process.hadronicCleanedDefLoose.clone(primaryVertices = cms.InputTag("vertexSelectorDefaultTight", "HadronicProcess"))

#process.hadrAndConvCleanedDefLoose = process.hadronicCleanedDefLoose.clone(primaryVertices = cms.InputTag("vertexSelectorDefaultLoose", "ConversionsProcess"), secondaryVertices = cms.InputTag("hadronicCleanedDefLoose"))

#process.hadrAndConvCleanedDefTight = process.hadrAndConvCleanedDefLoose.clone(primaryVertices = cms.InputTag("vertexSelectorDefaultTight", "ConversionsProcess"), secondaryVertices = cms.InputTag("hadronicCleanedDefTight"))

#process.hadrAndConvCleanedDefMedium = process.hadrAndConvCleanedDefLoose.clone(primaryVertices = cms.InputTag("vertexSelectorDefaultMedium", "ConversionsProcess"), secondaryVertices = cms.InputTag("hadronicCleanedDefMedium"))

#process.hadronicCleanedEditLoose = process.hadronicCleanedDefLoose.clone(primaryVertices = cms.InputTag("vertexSelectorEditLoose", "HadronicProcess"), secondaryVertices = cms.InputTag("inclusiveMergedVerticesEdit"))

#process.hadronicCleanedEditMedium = process.hadronicCleanedEditLoose.clone(primaryVertices = cms.InputTag("vertexSelectorEditMedium", "HadronicProcess"))

#process.hadronicCleanedEditTight = process.hadronicCleanedEditLoose.clone(primaryVertices = cms.InputTag("vertexSelectorEditTight", "HadronicProcess"))


#=============DEFINE ANALYSIS MODULES====================

process.treeMaker = cms.EDAnalyzer("IvfTreeMaker",
    primaryVertices = cms.InputTag("offlinePrimaryVertices"),
    secondaryVertices = cms.InputTag("inclusiveMergedVertices"),
    trackingParticles = cms.InputTag("mergedtruth","MergedTrackTruth"),
    debugMessage = cms.bool(False),
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


#=============DEFINE ANALYSIS MODULES====================

process.p = cms.Path(process.inclusiveVertexing * process.treeMaker)


process.TFileService = cms.Service("TFileService",
	fileName = cms.string("vertexTree_2.root")
)

#process.out = cms.OutputModule("PoolOutputModule",
	##naming convention: '<CMSSW PYTHON MODULE>_<RUN ON THAT DAY>' plus '_gc' if comes from grid-control job; in that case, leave out output directory path (see below)
	##=> how can you mark certain files (e.g. that show specific results)? only in configuration.txt file or in file name itself? or maybe in directory name?
	#fileName = cms.untracked.string('ivf_VertexHistOwn_3_allIdentifierMedium_withGeant.root'), # /nfs/dust/cms/user/nowatsd/Output/ivf_NImap/<DATE>
	#SelectEvents = cms.untracked.PSet(
		#SelectEvents = cms.vstring("pNoFilter"))
#)

#process.endpath = cms.EndPath(process.out)

#process.PoolSource.fileNames = [
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/000E24FB-5025-E211-91E3-0030487D5E9D.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/002A1E74-5D25-E211-843E-003048C693F8.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/007147F7-FA25-E211-ABE9-00266CFFB390.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/007686A5-5425-E211-A02A-00266CFFA780.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00B0D68D-5A25-E211-BA94-002481E0D398.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00B426A2-5625-E211-BED6-0025901D4B4C.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00BCB78E-5825-E211-B7F3-003048D436D2.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00C98A01-5B25-E211-BE79-00215AEDFD12.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00D1D3CC-7A25-E211-A0E9-0025904B141E.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00DD5739-6025-E211-A623-0030487E4B8F.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00DE7CF3-E825-E211-9610-00266CFFA23C.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00E123DC-4725-E211-82E5-00266CFB8D74.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/00E93433-4125-E211-A866-00266CF1074C.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02153AC3-5525-E211-AA53-003048D436D2.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02162455-E225-E211-9409-003048D4DFBC.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02354B69-5B25-E211-8744-003048D43838.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02472875-5625-E211-9013-003048C692FA.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/025C740B-5A25-E211-B824-003048F0E82C.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02AD950E-6C25-E211-B2B7-0025901D4C92.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02B9D84C-5725-E211-B4BC-0025904B1446.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/02D53154-6C25-E211-AC9D-003048F0E526.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/042DD7BE-4325-E211-AC97-00266CF32F18.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/047DE5F2-6A25-E211-9861-0025904B12DE.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0490531E-5A25-E211-A09A-00215AD4D6E2.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/04A0847D-5A25-E211-A95C-003048D47912.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/04C76AF6-6025-E211-AB72-00266CF33340.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/04E30D2B-6D25-E211-B1A1-003048D43726.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/04EC2A74-6C25-E211-B1C4-00266CFFA5CC.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/04F90F0E-4025-E211-A7EC-0030487D5EA7.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/060CBF34-5F25-E211-BCA7-0025901D4C18.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0682AEFF-5B25-E211-876A-00266CFFA344.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0693950B-6C25-E211-A37A-0025901D4B08.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/069759ED-EE25-E211-9176-0025901D4B20.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/06D1B831-4925-E211-B90D-0025904B1444.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/06D5BF80-6225-E211-94A1-00266CF9C1AC.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/085199F9-6925-E211-8A67-00266CF327C4.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0856436A-4025-E211-8B36-003048C692DA.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0873919E-5025-E211-B17D-002590494CB2.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/08A5F2D8-4625-E211-9141-003048D3C7DC.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/08D30598-5F25-E211-A5E9-003048F0E1B2.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/08DD1CD9-4325-E211-9E03-0025904B12DE.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0A21892F-5425-E211-BB74-002481E94B26.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0A4069CA-5F25-E211-A4E8-003048C692E0.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0A45034B-4025-E211-9CBF-0030487F1F2F.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0A79ACD3-6B25-E211-AD1A-003048F0E822.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0A7F7D9B-5925-E211-B5D4-003048D4DFAA.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0A8CB1DF-5B25-E211-9C20-0025901D4AF0.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0AB2C3D9-5B25-E211-BAE1-003048D4397E.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0AB9E748-4425-E211-820C-00266CFFB1F4.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0C28F18B-F425-E211-8D5B-0030487F91DB.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0C2F71CC-5A25-E211-8EC5-002481E0D2E8.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0C3CAD7A-6D25-E211-8102-0030487D7103.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0C58EF24-5625-E211-9B9A-003048D47912.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0C5AFABC-6025-E211-9ADE-0025901D4844.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0C661F5B-5A25-E211-8BD6-003048F0EBB8.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0CF054DC-5F25-E211-B6B2-003048C66184.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0E3EE564-5625-E211-8E37-0030487D5EA7.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0E956A44-3625-E211-AF15-002590494CE4.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0EC6F882-5B25-E211-A192-00266CF2AE10.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0EC86B4A-6025-E211-8374-00266CF3336C.root",
  #"/store/mc/Summer12_DR53X/TTJets_MassiveBinDECAY_TuneZ2star_8TeV-madgraph-tauola/GEN-SIM-RECODEBUG/PU_S10_START53_V7C-v1/00000/0ED174D2-5C25-E211-A30B-0030487D5DBD.root"
#]



