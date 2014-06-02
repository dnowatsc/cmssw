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

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring()
)

process.GlobalTag.globaltag = 'START53_V27::All'

#=============FILTER OUT TRACKS FROM PRIMARY VERTICES====================

process.filteredTracks = cms.EDProducer("TrackPrimaryVertexFilter",
	primaryVertices = cms.InputTag("offlinePrimaryVertices"),
	tracks = cms.InputTag("generalTracks")
)

#=============STORE TRACKS FROM SECONDARY VERTICES====================

#process.svTracksDefault = cms.EDProducer("TrackFilterFromSecondaryVertices",
	#secondaryVertices = cms.InputTag("inclusiveMergedVertices")
#)

#process.svTracksEdit = process.svTracksDefault.clone( secondaryVertices = cms.InputTag("inclusiveMergedVerticesEdit") )

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

process.vertexSelectorDefault = cms.EDFilter("VertexHistoryFilter",
    vertexConfig = cms.PSet(process.vertexClassifier),
    #secondaryVertexTagInfoCollection = cms.InputTag('secondaryVertexTagInfos'),
    vertices = cms.InputTag('inclusiveMergedVertices'),
    #category = cms.string("HadronicProcess")
)



process.vertexSelectorDefault.vertexConfig.vertexProducer = cms.untracked.InputTag("inclusiveMergedVertices")

process.vertexSelectorEdit = process.vertexSelectorDefault.clone( vertices = cms.InputTag('inclusiveMergedVerticesEdit') )

#process.vertexSelectorDefault.vertexConfig.trackProducer = cms.untracked.InputTag("svTracksDefault")

#process.vertexSelectorEdit.vertexConfig.trackProducer = cms.untracked.InputTag("svTracksEdit")

#===========COLLECT SECONDARY VERTICES=====================

process.svcollectorHadron = cms.EDFilter("SecondaryVertexCollector",
	MaxCount = cms.int32(100), #__MAX_EVENTS__
	VertexInput = cms.InputTag("vertexSelectorDefault", "HadronicProcess")
	#GenParticleInput = cms.InputTag("genParticles")
)

process.svcollectorMuNucl = process.svcollectorHadron.clone( VertexInput = cms.InputTag("vertexSelectorDefault", "MuNuclProcess") )

process.svcollectorUnknown = process.svcollectorHadron.clone( VertexInput = cms.InputTag("vertexSelectorDefault", "UnknownProcess") )

process.svcollectorUndefined = process.svcollectorHadron.clone( VertexInput = cms.InputTag("vertexSelectorDefault", "UndefinedProcess") )

process.svcollectorHadronEdit = process.svcollectorHadron.clone( VertexInput = cms.InputTag("vertexSelectorEdit", "HadronicProcess") )

process.svcollectorMuNuclEdit = process.svcollectorHadron.clone( VertexInput = cms.InputTag("vertexSelectorEdit", "MuNuclProcess") )

process.svcollectorUnknownEdit = process.svcollectorHadron.clone( VertexInput = cms.InputTag("vertexSelectorEdit", "UnknownProcess") )

process.svcollectorUndefinedEdit = process.svcollectorHadron.clone( VertexInput = cms.InputTag("vertexSelectorEdit", "UndefinedProcess") )

process.svcollectorBDecay = process.svcollectorHadron.clone( VertexInput = cms.InputTag("vertexSelectorDefault", "BWeakDecay") )

process.svcollectorCDecay = process.svcollectorHadron.clone( VertexInput = cms.InputTag("vertexSelectorDefault", "CWeakDecay") )

process.svcollectorKShort = process.svcollectorHadron.clone( VertexInput = cms.InputTag("vertexSelectorDefault", "KsDecay") )

process.svcollectorBDecayEdit = process.svcollectorHadron.clone( VertexInput = cms.InputTag("vertexSelectorEdit", "BWeakDecay") )

process.svcollectorCDecayEdit = process.svcollectorHadron.clone( VertexInput = cms.InputTag("vertexSelectorEdit", "CWeakDecay") )

process.svcollectorKShortEdit = process.svcollectorHadron.clone( VertexInput = cms.InputTag("vertexSelectorEdit", "KsDecay") )

process.svcollectorNoFilter = process.svcollectorHadron.clone( VertexInput = cms.InputTag("inclusiveMergedVertices") )


#===========DEFINE THE PATHS=====================


process.pNoFilter = cms.Path(process.inclusiveVertexing * process.svcollectorNoFilter)

process.pHadron = cms.Path(process.inclusiveVertexing * process.vertexSelectorDefault * process.svcollectorHadron)

process.pHadronEdit = cms.Path(process.filteredTracks * process.inclusiveVertexingEdit * process.vertexSelectorEdit * process.svcollectorHadronEdit)

process.pMuNucl = cms.Path(process.inclusiveVertexing * process.vertexSelectorDefault * process.svcollectorMuNucl)

process.pMuNuclEdit = cms.Path(process.filteredTracks * process.inclusiveVertexingEdit * process.vertexSelectorEdit * process.svcollectorMuNuclEdit)

process.pUnknown = cms.Path(process.inclusiveVertexing * process.vertexSelectorDefault * process.svcollectorUnknown)

process.pUnknownEdit = cms.Path(process.filteredTracks * process.inclusiveVertexingEdit * process.vertexSelectorEdit * process.svcollectorUnknownEdit)

process.pUndefined = cms.Path(process.inclusiveVertexing * process.vertexSelectorDefault * process.svcollectorUndefined)

process.pUndefinedEdit = cms.Path(process.filteredTracks * process.inclusiveVertexingEdit * process.vertexSelectorEdit * process.svcollectorUndefinedEdit)

process.pBDecay = cms.Path(process.inclusiveVertexing * process.vertexSelectorDefault * process.svcollectorBDecay)

process.pBDecayEdit = cms.Path(process.filteredTracks * process.inclusiveVertexingEdit * process.vertexSelectorEdit * process.svcollectorBDecayEdit)

process.pCDecay = cms.Path(process.inclusiveVertexing * process.vertexSelectorDefault * process.svcollectorCDecay)

process.pCDecayEdit = cms.Path(process.filteredTracks * process.inclusiveVertexingEdit * process.vertexSelectorEdit * process.svcollectorCDecayEdit)

process.pKShort = cms.Path(process.inclusiveVertexing * process.vertexSelectorDefault * process.svcollectorKShort)

process.pKShortEdit = cms.Path(process.filteredTracks * process.inclusiveVertexingEdit * process.vertexSelectorEdit * process.svcollectorKShortEdit)


process.out = cms.OutputModule("PoolOutputModule",
	#naming convention: '<CMSSW PYTHON MODULE>_<RUN ON THAT DAY>' plus '_gc' if comes from grid-control job; in that case, leave out output directory path (see below)
	#=> how can you mark certain files (e.g. that show specific results)? only in configuration.txt file or in file name itself? or maybe in directory name?
	fileName = cms.untracked.string('ivf_VertexHist_2.root'), # /nfs/dust/cms/user/nowatsd/Output/ivf_NImap/<DATE>
	outputCommands = cms.untracked.vstring('drop *', 'keep *_svcollector*_*_*'),
	SelectEvents = cms.untracked.PSet(
		SelectEvents = cms.vstring("pNoFilter"))
)

process.endpath= cms.EndPath(process.out)

process.PoolSource.fileNames = [
  "file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_5_3_14/src/Validation/RecoB/input_rootfiles/ttjets_8tev_mad_recodebug.root",
  
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



