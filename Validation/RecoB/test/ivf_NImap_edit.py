import FWCore.ParameterSet.Config as cms

process = cms.Process("validation")
# load the full reconstraction configuration, to make sure we're getting all needed dependencies
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")

#process.load("PhysicsTools.JetMCAlgos.CaloJetsMCFlavour_cfi")
process.load("RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff")

# process.load("SimTracker.TrackHistory.TrackClassifier_cff")

process.GlobalTag.globaltag = 'START53_V27::All'

process.maxEvents = cms.untracked.PSet(
	input = cms.untracked.int32(__MAX_EVENTS__) #__MAX_EVENTS__
)

#=============RECONSTRUCT THE SECONDARY VERTICES====================

process.inclusiveVertexFinderEdit = process.inclusiveVertexFinder.clone(
	#beamSpot = cms.InputTag("offlineBeamSpot"),
	#primaryVertices = cms.InputTag("offlinePrimaryVertices"),
	#tracks = cms.InputTag("generalTracks"),
	minHits = cms.uint32(6), #default 8
	maximumLongitudinalImpactParameter = cms.double(2.0), #default 0.3
	minPt = cms.double(0.6), # default 0.8
	maxNTracks = cms.uint32(30),
	
	clusterizer = cms.PSet(
		seedMin3DIPSignificance = cms.double(1.2),
		seedMin3DIPValue = cms.double(0.005),
		clusterMaxDistance = cms.double(0.05), #500um
		clusterMaxSignificance = cms.double(4.5), #4.5 sigma
		clusterScale = cms.double(1), 
		clusterMinAngleCosine = cms.double(0.5), # only forward decays
	),

	vertexMinAngleCosine = cms.double(0.7), # scalar prod direction of tracks and flight dir
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
	secondaryVertices = cms.InputTag("vertexMergerEdit")
)

process.inclusiveMergedVerticesEdit = process.inclusiveMergedVertices.clone(
	secondaryVertices = cms.InputTag("trackVertexArbitratorEdit")
)

process.inclusiveVertexingEdit = cms.Sequence(process.inclusiveVertexFinderEdit * process.vertexMergerEdit * process.trackVertexArbitratorEdit * process.inclusiveMergedVerticesEdit)

#=============APPLY NUCLEARINTERACTIONIDENTIFIER TO ALL 4 SETS OF RECONSTRUCTED SECONDARY VERTICES====================

process.nuclearInteractionIdentifierDef = cms.EDProducer("NuclearInteractionIdentifier",
	primaryVertices = cms.InputTag("offlinePrimaryVertices"),
	secondaryVertices = cms.InputTag("inclusiveMergedVertices"),
	beamSpot = cms.InputTag("offlineBeamSpot")
)

process.nuclearInteractionIdentifierEdit = process.nuclearInteractionIdentifierDef.clone(secondaryVertices = cms.InputTag("inclusiveMergedVerticesEdit"))


#=============REMOVE ALL NUCLEAR INTERACION VERTICES USING VERTEXCLEANER====================

#process.cleanedInclusiveMergedVerticesDef = cms.EDProducer("VertexCleaner",
	#primaryVertices= cms.InputTag("nuclearInteractionIdentifierDef"),
	#secondaryVertices = cms.InputTag("inclusiveMergedVertices"),
	#maxFraction = cms.double(0.0)
#)

#process.cleanedInclusiveMergedVerticesEdit = process.cleanedInclusiveMergedVerticesDef.clone(primaryVertices = cms.InputTag("nuclearInteractionIdentifierEdit"), secondaryVertices = cms.InputTag("inclusiveMergedVerticesEdit"))

#=============COLLECT THE SECONDARY VERTICES AND STORE IN ONE EVENT====================

process.svcollectorEdit = cms.EDFilter("SecondaryVertexCollector",
	MaxCount = cms.int32(__MAX_EVENTS__), #__MAX_EVENTS__
	VertexInput = cms.InputTag("inclusiveMergedVerticesEdit"),
	GenParticleInput = cms.InputTag("genParticles")
)

process.svcollectorDef = process.svcollectorEdit.clone(VertexInput = cms.InputTag("inclusiveMergedVertices"))

process.svcollectorEditNuc = process.svcollectorEdit.clone(VertexInput = cms.InputTag("nuclearInteractionIdentifierEdit"))

process.svcollectorDefNuc = process.svcollectorEdit.clone(VertexInput = cms.InputTag("nuclearInteractionIdentifierDef"))


process.pDef = cms.Path(process.inclusiveVertexing * process.svcollectorDef)

process.pEdit = cms.Path(process.inclusiveVertexingEdit * process.svcollectorEdit)

process.pDefNuc = cms.Path(process.inclusiveVertexing * process.nuclearInteractionIdentifierDef * process.svcollectorDefNuc)

process.pEditNuc = cms.Path(process.inclusiveVertexingEdit * process.nuclearInteractionIdentifierEdit * process.svcollectorEditNuc)
#process.svmapDef = cms.Path(process.inclusiveVertexing * process.svcollectorDef)


process.source = cms.Source("PoolSource",
fileNames = cms.untracked.vstring(__FILE_NAMES__), #__FILE_NAMES__
	skipEvents = cms.untracked.uint32(__SKIP_EVENTS__)
)

process.outEdit = cms.OutputModule("PoolOutputModule",
	fileName = cms.untracked.string('secondary_vertex_map_9_Edit_minHits6MLIP2minpt06vminang07_NIcollector.root'),
	SelectEvents = cms.untracked.PSet(
		SelectEvents = cms.vstring("pEdit"))
)

#process.outDef = cms.OutputModule("PoolOutputModule",
	#fileName = cms.untracked.string('/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_5_3_14/src/Validation/RecoB/rootfiles/secondary_vertex_map_4_Def.root'),
	#SelectEvents = cms.untracked.PSet(
		#SelectEvents = cms.vstring("svmapDef"))
#)

process.endpath= cms.EndPath(process.outEdit) #*process.outDef

#process.source.fileNames = [
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/000FFBED-DA11-E211-B882-00A0D1EE8B08.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/002B4D6D-1111-E211-AE43-00A0D1EE9644.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/003BE66F-B912-E211-9264-00A0D1EE8A20.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/006251FE-C012-E211-9B39-00266CF9BF5C.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/009B0759-2811-E211-ABD5-00266CFAE228.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/00C3913C-7411-E211-90E4-848F69FD2928.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/00D9B869-E611-E211-B87B-0024E8768299.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/027A827A-5511-E211-9A88-00A0D1EEF328.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/02B0A4C1-2312-E211-9B1D-00266CF25878.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/02CC8FDF-1212-E211-BA50-0024E8768D68.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/02E19FB1-BD10-E211-84FC-008CFA0021D4.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/02E86910-FB11-E211-B003-0024E8768D41.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/044CA6FD-7412-E211-B8E4-00266CF25998.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/045176A2-AF12-E211-8DFB-848F69FD4568.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/04662905-1C12-E211-AA36-848F69FD2823.root"
#]