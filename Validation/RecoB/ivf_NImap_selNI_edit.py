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
	input = cms.untracked.int32(1000) #__MAX_EVENTS__
)

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

#=============APPLY NUCLEARINTERACTIONIDENTIFIER TO ALL 4 SETS OF RECONSTRUCTED SECONDARY VERTICES====================

process.nuclearInteractionIdentifierDef = cms.EDProducer("NuclearInteractionIdentifierDetector",
	primaryVertices = cms.InputTag("offlinePrimaryVertices"),
	secondaryVertices = cms.InputTag("inclusiveMergedVertices"),
	beamSpot = cms.InputTag("offlineBeamSpot")
)

process.nuclearInteractionIdentifierEdit = process.nuclearInteractionIdentifierDef.clone(secondaryVertices = cms.InputTag("inclusiveMergedVerticesEdit"))


#=============REMOVE ALL NUCLEAR INTERACION VERTICES USING VERTEXCLEANER====================

process.cleanedInclusiveMergedVerticesDef = cms.EDProducer("VertexCleaner",
	primaryVertices= cms.InputTag("nuclearInteractionIdentifierDef"),
	secondaryVertices = cms.InputTag("inclusiveMergedVertices"),
	maxFraction = cms.double(0.0)
)

process.cleanedInclusiveMergedVerticesEdit = process.cleanedInclusiveMergedVerticesDef.clone(primaryVertices = cms.InputTag("nuclearInteractionIdentifierEdit"), secondaryVertices = cms.InputTag("inclusiveMergedVerticesEdit"))

#=============COLLECT THE SECONDARY VERTICES AND STORE IN ONE EVENT====================

process.svcollectorEdit = cms.EDFilter("SecondaryVertexCollector",
	MaxCount = cms.int32(1000), #__MAX_EVENTS__
	VertexInput = cms.InputTag("inclusiveMergedVerticesEdit")
	#GenParticleInput = cms.InputTag("genParticles")
)

process.svcollectorDef = process.svcollectorEdit.clone(VertexInput = cms.InputTag("inclusiveMergedVertices"))

process.svcollectorEditNucColl = process.svcollectorEdit.clone(VertexInput = cms.InputTag("nuclearInteractionIdentifierEdit"))

process.svcollectorDefNucColl = process.svcollectorEdit.clone(VertexInput = cms.InputTag("nuclearInteractionIdentifierDef"))

process.svcollectorEditNucRej = process.svcollectorEdit.clone(VertexInput = cms.InputTag("cleanedInclusiveMergedVerticesEdit"))

process.svcollectorDefNucRej = process.svcollectorEdit.clone(VertexInput = cms.InputTag("cleanedInclusiveMergedVerticesDef"))

#=============TEST REASON FOR LARGE NUMBER OF SVS CLOSE TO THE BEAM AXIS====================

process.svcollectorEditFirst = process.svcollectorEdit.clone(VertexInput = cms.InputTag("inclusiveVertexFinderEdit"))

process.svcollectorEditSecond = process.svcollectorEdit.clone(VertexInput = cms.InputTag("vertexMergerEdit"))

process.svcollectorEditThird = process.svcollectorEdit.clone(VertexInput = cms.InputTag("trackVertexArbitratorEdit"))

process.pEditFirst = cms.Path(process.filteredTracks * process.inclusiveVertexingEdit * process.svcollectorEditFirst)

process.pEditSecond = cms.Path(process.filteredTracks * process.inclusiveVertexingEdit * process.svcollectorEditSecond)

process.pEditThird = cms.Path(process.filteredTracks * process.inclusiveVertexingEdit * process.svcollectorEditThird)

#=============EXECUTE THE MODULES====================

process.pDef = cms.Path(process.inclusiveVertexing * process.svcollectorDef)

process.pEdit = cms.Path(process.filteredTracks * process.inclusiveVertexingEdit * process.svcollectorEdit)

process.pDefNucColl = cms.Path(process.inclusiveVertexing * process.nuclearInteractionIdentifierDef * process.svcollectorDefNucColl)

process.pEditNucColl = cms.Path(process.filteredTracks * process.inclusiveVertexingEdit * process.nuclearInteractionIdentifierEdit * process.svcollectorEditNucColl)

process.pDefNucRej = cms.Path(process.inclusiveVertexing * process.nuclearInteractionIdentifierDef * process.cleanedInclusiveMergedVerticesDef * process.svcollectorDefNucRej)

process.pEditNucRej = cms.Path(process.filteredTracks * process.inclusiveVertexingEdit * process.nuclearInteractionIdentifierEdit * process.cleanedInclusiveMergedVerticesEdit * process.svcollectorEditNucRej)

#process.svmapDef = cms.Path(process.inclusiveVertexing * process.svcollectorDef)


process.source = cms.Source("PoolSource",
	fileNames = cms.untracked.vstring() #__FILE_NAMES__
	#skipEvents = cms.untracked.uint32(__SKIP_EVENTS__)
)

process.outEdit = cms.OutputModule("PoolOutputModule",
	#naming convention: '<CMSSW PYTHON MODULE>_<RUN ON THAT DAY>' plus '_gc' if comes from grid-control job; in that case, leave out output directory path (see below)
	#=> how can you mark certain files (e.g. that show specific results)? only in configuration.txt file or in file name itself? or maybe in directory name?
	fileName = cms.untracked.string('/nfs/dust/cms/user/nowatsd/Output/ivf_NImap/14-01-22/ivf_NImap_selNI_TEST_0.root'), # /nfs/dust/cms/user/nowatsd/Output/ivf_NImap/<DATE>
	SelectEvents = cms.untracked.PSet(
		SelectEvents = cms.vstring("pEdit"))
)

#process.outDef = cms.OutputModule("PoolOutputModule",
	#fileName = cms.untracked.string('/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_5_3_14/src/Validation/RecoB/rootfiles/secondary_vertex_map_4_Def.root'),
	#SelectEvents = cms.untracked.PSet(
		#SelectEvents = cms.vstring("svmapDef"))
#)

process.endpath= cms.EndPath(process.outEdit) #*process.outDef

process.source.fileNames = [
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/000FFBED-DA11-E211-B882-00A0D1EE8B08.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/002B4D6D-1111-E211-AE43-00A0D1EE9644.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/003BE66F-B912-E211-9264-00A0D1EE8A20.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/006251FE-C012-E211-9B39-00266CF9BF5C.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/009B0759-2811-E211-ABD5-00266CFAE228.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/00C3913C-7411-E211-90E4-848F69FD2928.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/00D9B869-E611-E211-B87B-0024E8768299.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/027A827A-5511-E211-9A88-00A0D1EEF328.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/02B0A4C1-2312-E211-9B1D-00266CF25878.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/02CC8FDF-1212-E211-BA50-0024E8768D68.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/02E19FB1-BD10-E211-84FC-008CFA0021D4.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/02E86910-FB11-E211-B003-0024E8768D41.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/044CA6FD-7412-E211-B8E4-00266CF25998.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/045176A2-AF12-E211-8DFB-848F69FD4568.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/04662905-1C12-E211-AA36-848F69FD2823.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/04662F83-E712-E211-BF15-00266CF268B8.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/046D1088-9B12-E211-838D-008CFA000744.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/04827982-A611-E211-B849-848F69FD28C8.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/04E17911-FF10-E211-9867-00266CF9B04C.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/04E70BEC-1212-E211-81A8-00A0D1EE25D0.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/04FA6F4E-DF11-E211-B7D1-848F69FD5027.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/06410C50-BC11-E211-B67A-00266CFAEA68.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/06483FE2-FF10-E211-AF0D-848F69FD45A7.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/069F36FB-0F12-E211-BD08-848F69FD4E14.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/069FBF82-8810-E211-A84E-00266CF9B04C.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/06ADC0D0-7511-E211-BBFC-00266CFAE69C.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/06C6F35C-FC11-E211-A03D-00266CF9AD20.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/08284FEF-DC11-E211-BBBD-0024E8769965.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0842AA8F-CB12-E211-A5BA-0024E876A7C6.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0854B17B-5312-E211-B293-008CFA0021D4.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0856427C-4311-E211-BB1F-0026B94D1AD5.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/08A2FD23-2111-E211-AE50-008CFA002490.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/08A5558B-2612-E211-8AFF-00A0D1EEF5B4.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/08ABCBEA-C212-E211-9E2C-0024E8769B39.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/08C53758-AC12-E211-930A-00266CF9B684.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/08D60329-0A11-E211-88BB-848F69FD2520.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0A689B53-AE11-E211-A719-848F69FD289B.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0A862B6E-AD12-E211-AC79-848F69FD25BC.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0AAF585D-1812-E211-BA12-848F69FD289B.root",
"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0ABF8933-C511-E211-BB61-0026B94D1AEF.root"
]