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

process.load("Validation.RecoB.bTagAnalysis_cfi")
process.bTagValidation.jetMCSrc = 'AK5byValAlgo'
process.bTagValidation.allHistograms = True 
#process.bTagValidation.fastMC = True

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(__MAX_EVENTS__)
)
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(__FILE_NAMES__),
    skipEvents = cms.untracked.uint32(__SKIP_EVENTS__)
)

process.GlobalTag.globaltag = 'START53_V27::All'

#=============FILTER TRACKS FROM PRIMARY VERTICES====================

process.filteredTracks = cms.EDProducer("TrackPrimaryVertexFilter",
	primaryVertices = cms.InputTag("offlinePrimaryVertices"),
	tracks = cms.InputTag("generalTracks")
)

#===========CONFIGURE PERFORMANCE PLOTTER=========================

process.bTagValidation.tagConfig.append(cms.PSet(
            parameters = cms.PSet(
        			discriminatorStart = cms.double(-0.05),
        			discriminatorEnd = cms.double(1.05),
        			nBinEffPur = cms.int32(200),
        			# the constant b-efficiency for the differential plots versus pt and eta
        			effBConst = cms.double(0.5),
        			endEffPur = cms.double(1.005),
        			startEffPur = cms.double(-0.005)
  				),
            label = cms.InputTag("combinedInclusiveSecondaryVertexBJetTags"),
            folder = cms.string("CSVIVF")
)
)

process.CustombTagValidation = process.bTagValidation.clone(
    tagConfig = cms.VPSet(
        cms.PSet(
				    parameters = cms.PSet(
        			discriminatorStart = cms.double(-0.05),
        			discriminatorEnd = cms.double(1.05),
        			nBinEffPur = cms.int32(200),
        			# the constant b-efficiency for the differential plots versus pt and eta
        			effBConst = cms.double(0.5),
        			endEffPur = cms.double(1.005),
        			startEffPur = cms.double(-0.005)
    				),
            label = cms.InputTag("combinedSecondaryVertexBJetTags"),
            folder = cms.string("CSV")
        ),  
        cms.PSet(
            parameters = cms.PSet(
                                discriminatorStart = cms.double(-0.05),
                                discriminatorEnd = cms.double(1.05),
                                nBinEffPur = cms.int32(200),
                                # the constant b-efficiency for the differential plots versus pt and eta
                                effBConst = cms.double(0.5),
                                endEffPur = cms.double(1.005),
                                startEffPur = cms.double(-0.005)
                                ),
            label = cms.InputTag("combinedInclusiveSecondaryVertexBJetTags"),
            folder = cms.string("CSVIVF")
           ),  
        cms.PSet(
            parameters = cms.PSet(
                                discriminatorStart = cms.double(-0.05),
                                discriminatorEnd = cms.double(1.05),
                                nBinEffPur = cms.int32(200),
                                # the constant b-efficiency for the differential plots versus pt and eta
                                effBConst = cms.double(0.5),
                                endEffPur = cms.double(1.005),
                                startEffPur = cms.double(-0.005)
                                ),
            label = cms.InputTag("cleanedCombinedInclusiveSecondaryVertexBJetTags"),
            folder = cms.string("CSVIVF-NI")
           ),
       cms.PSet(
            parameters = cms.PSet(
                                discriminatorStart = cms.double(-0.05),
                                discriminatorEnd = cms.double(1.05),
                                nBinEffPur = cms.int32(200),
                                # the constant b-efficiency for the differential plots versus pt and eta
                                effBConst = cms.double(0.5),
                                endEffPur = cms.double(1.005),
                                startEffPur = cms.double(-0.005)
                                ),
                                label = cms.InputTag("cleanedCombinedInclusiveSecondaryVertexBJetTagsOwn"),
            folder = cms.string("CSVIVF-NI-OWN")
           ),      
       cms.PSet(
            parameters = cms.PSet(
                                discriminatorStart = cms.double(-0.05),
                                discriminatorEnd = cms.double(1.05),
                                nBinEffPur = cms.int32(200),
                                # the constant b-efficiency for the differential plots versus pt and eta
                                effBConst = cms.double(0.5),
                                endEffPur = cms.double(1.005),
                                startEffPur = cms.double(-0.005)
                                ),
                                label = cms.InputTag("cleanedCombinedInclusiveSecondaryVertexBJetTagsNoPu"),
            folder = cms.string("CSVIVF-NI-NOPU")
           )

       )

)
process.CustombTagValidation.ptRecJetMin = cms.double(20.0)
process.CustombTagValidation.ptRanges = cms.vdouble(20, 30, 50.0, 80.0, 120.0,300, 600)


#==============SET UP DEFAULT IVF==============================

process.load("RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff")

#==============SET UP ANDREA RIZZI'S NUCLEAR INTERACTION FINDER==============================

process.nuclearInteractionIdentifier = cms.EDProducer("NuclearInteractionIdentifier",
     primaryVertices = cms.InputTag("offlinePrimaryVertices"),
     secondaryVertices = cms.InputTag("inclusiveMergedVertices"),
     beamSpot = cms.InputTag("offlineBeamSpot")
)

process.cleanedInclusiveMergedVertices = cms.EDProducer("VertexCleaner",
        primaryVertices= cms.InputTag("nuclearInteractionIdentifier"),
        secondaryVertices = cms.InputTag("inclusiveMergedVertices"),
        maxFraction = cms.double(0.0)
)

process.trackCollectionCleaner = cms.EDProducer("TrackCollectionCleaner",
        vertices= cms.InputTag("nuclearInteractionIdentifier"),
        tracks = cms.InputTag("generalTracks")
)


process.ak5JetCleanedTracksAssociatorAtVertex = process.ak5JetTracksAssociatorAtVertex.clone()
process.ak5JetCleanedTracksAssociatorAtVertex.tracks = cms.InputTag("trackCollectionCleaner")


process.inclusiveVertexFinder2 = process.inclusiveVertexFinder.clone(tracks = cms.InputTag("trackCollectionCleaner"))
process.vertexMerger2 = process.vertexMerger.clone(secondaryVertices = cms.InputTag("inclusiveVertexFinder2"))
process.trackVertexArbitrator2=process.trackVertexArbitrator.clone(tracks = cms.InputTag("trackCollectionCleaner"),secondaryVertices = cms.InputTag("vertexMerger2"))
process.inclusiveMergedVertices2= process.inclusiveMergedVertices.clone(secondaryVertices = cms.InputTag("trackVertexArbitrator2"))

process.inclusiveVertexing2 = cms.Sequence(process.inclusiveVertexFinder2*process.vertexMerger2*process.trackVertexArbitrator2*process.inclusiveMergedVertices2)

#new
process.offlinePrimaryVertices2 = process.offlinePrimaryVertices.clone(TrackLabel=cms.InputTag("trackCollectionCleaner"))
process.inclusiveVertexFinder2.primaryVertices = cms.InputTag("offlinePrimaryVertices2")
process.trackVertexArbitrator2.primaryVertices = cms.InputTag("offlinePrimaryVertices2")

process.cleanedImpactParameterTagInfos = process.impactParameterTagInfos.clone()
process.cleanedImpactParameterTagInfos.jetTracks = cms.InputTag("ak5JetCleanedTracksAssociatorAtVertex")
process.cleanedImpactParameterTagInfos.primaryVertex = cms.InputTag("offlinePrimaryVertices2")


process.cleanedInclusiveSecondaryVertexFinderTagInfos = process.inclusiveSecondaryVertexFinderTagInfos.clone(
        extSVCollection = cms.InputTag("inclusiveMergedVertices2"),
        trackIPTagInfos = cms.InputTag("cleanedImpactParameterTagInfos")
)
process.cleanedCombinedInclusiveSecondaryVertexBJetTags = process.combinedInclusiveSecondaryVertexBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag("cleanedImpactParameterTagInfos"),
                                 cms.InputTag("cleanedInclusiveSecondaryVertexFinderTagInfos"))
)

#==============SET UP OWN NUCLEAR INTERACTION FINDER==============================

process.nuclearInteractionIdentifierOwn = cms.EDProducer("NuclearInteractionIdentifierDetector",
	primaryVertices = cms.InputTag("offlinePrimaryVertices"),
	secondaryVertices = cms.InputTag("inclusiveMergedVertices"),
	beamSpot = cms.InputTag("offlineBeamSpot")
)

process.cleanedInclusiveMergedVerticesOwn = cms.EDProducer("VertexCleaner",
	primaryVertices= cms.InputTag("nuclearInteractionIdentifierOwn"),
	secondaryVertices = cms.InputTag("inclusiveMergedVertices"),
	maxFraction = cms.double(0.0)
)

process.trackCollectionCleanerOwn = cms.EDProducer("TrackCollectionCleaner",
	vertices= cms.InputTag("nuclearInteractionIdentifierOwn"),
	tracks = cms.InputTag("generalTracks")
)

process.ak5JetCleanedTracksAssociatorAtVertexOwn = process.ak5JetTracksAssociatorAtVertex.clone(
	tracks = cms.InputTag("trackCollectionCleanerOwn")
)


process.inclusiveVertexFinder3 = process.inclusiveVertexFinder.clone(tracks = cms.InputTag("trackCollectionCleanerOwn"))
process.vertexMerger3 = process.vertexMerger.clone(secondaryVertices = cms.InputTag("inclusiveVertexFinder3"))
process.trackVertexArbitrator3 = process.trackVertexArbitrator.clone(tracks = cms.InputTag("trackCollectionCleanerOwn"),secondaryVertices = cms.InputTag("vertexMerger3"))
process.inclusiveMergedVertices3 = process.inclusiveMergedVertices.clone(secondaryVertices = cms.InputTag("trackVertexArbitrator3"))

process.inclusiveVertexing3 = cms.Sequence(process.inclusiveVertexFinder3*process.vertexMerger3*process.trackVertexArbitrator3*process.inclusiveMergedVertices3)

#new
process.offlinePrimaryVertices3 = process.offlinePrimaryVertices.clone(TrackLabel=cms.InputTag("trackCollectionCleanerOwn"))
process.inclusiveVertexFinder3.primaryVertices = cms.InputTag("offlinePrimaryVertices3")
process.trackVertexArbitrator3.primaryVertices = cms.InputTag("offlinePrimaryVertices3")

process.cleanedImpactParameterTagInfosOwn = process.impactParameterTagInfos.clone(
	jetTracks = cms.InputTag("ak5JetCleanedTracksAssociatorAtVertexOwn"),
	primaryVertex = cms.InputTag("offlinePrimaryVertices3")
)

process.cleanedInclusiveSecondaryVertexFinderTagInfosOwn = process.inclusiveSecondaryVertexFinderTagInfos.clone(
	extSVCollection = cms.InputTag("inclusiveMergedVertices3"),
	trackIPTagInfos = cms.InputTag("cleanedImpactParameterTagInfosOwn")
)
process.cleanedCombinedInclusiveSecondaryVertexBJetTagsOwn = process.combinedInclusiveSecondaryVertexBJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("cleanedImpactParameterTagInfosOwn"),
		cms.InputTag("cleanedInclusiveSecondaryVertexFinderTagInfosOwn"))
)


#==============SET UP IVF WITHOUT TRACKS FROM PRIMARY VERTEX COLLECTION==================

process.inclusiveVertexFinder4 = process.inclusiveVertexFinder.clone(tracks = cms.InputTag("filteredTracks"))
process.vertexMerger4 = process.vertexMerger.clone(secondaryVertices = cms.InputTag("inclusiveVertexFinder4"))
process.trackVertexArbitrator4 = process.trackVertexArbitrator.clone(tracks = cms.InputTag("filteredTracks"),secondaryVertices = cms.InputTag("vertexMerger4"))
process.inclusiveMergedVertices4 = process.inclusiveMergedVertices.clone(secondaryVertices = cms.InputTag("trackVertexArbitrator4"))

process.inclusiveVertexing4 = cms.Sequence(process.inclusiveVertexFinder4*process.vertexMerger4*process.trackVertexArbitrator4*process.inclusiveMergedVertices4)

process.cleanedInclusiveSecondaryVertexFinderTagInfosNoPu = process.inclusiveSecondaryVertexFinderTagInfos.clone(
	extSVCollection = cms.InputTag("inclusiveMergedVertices4")
)
process.cleanedCombinedInclusiveSecondaryVertexBJetTagsNoPu = process.combinedInclusiveSecondaryVertexBJetTags.clone(
        tagInfos = cms.VInputTag(cms.InputTag("impactParameterTagInfos"),
                                 cms.InputTag("cleanedInclusiveSecondaryVertexFinderTagInfosNoPu"))
)

#==============SET UP THE PATHS AND EXECUTE ALL THE MODULES==============================

#process.plots = cms.Path(process.nuclearInteractionIdentifier * process.cleanedInclusiveMergedVertices * process.trackCollectionCleaner * process.inclusiveVertexing2 * process.ak5JetCleanedTracksAssociatorAtVertex * process.cleanedImpactParameterTagInfos * process.cleanedInclusiveSecondaryVertexFinderTagInfos * process.cleanedCombinedInclusiveSecondaryVertexBJetTags * process.cleaned1InclusiveSecondaryVertexFinderTagInfos * process.cleaned1CombinedInclusiveSecondaryVertexBJetTags)

process.plots = cms.Path(process.filteredTracks * process.myPartons* process.AK5Flavour * process.btagging  * process.inclusiveVertexing * process.inclusiveSecondaryVertexFinderTagInfos * process.combinedInclusiveSecondaryVertexBJetTags * process.nuclearInteractionIdentifier * process.cleanedInclusiveMergedVertices * process.trackCollectionCleaner * process.offlinePrimaryVertices2 * process.inclusiveVertexing2 * process.ak5JetCleanedTracksAssociatorAtVertex * process.cleanedImpactParameterTagInfos * process.cleanedInclusiveSecondaryVertexFinderTagInfos * process.cleanedCombinedInclusiveSecondaryVertexBJetTags * process.nuclearInteractionIdentifierOwn * process.cleanedInclusiveMergedVerticesOwn * process.trackCollectionCleanerOwn * process.offlinePrimaryVertices3 * process.inclusiveVertexing3 * process.ak5JetCleanedTracksAssociatorAtVertexOwn * process.cleanedImpactParameterTagInfosOwn * process.cleanedInclusiveSecondaryVertexFinderTagInfosOwn * process.cleanedCombinedInclusiveSecondaryVertexBJetTagsOwn * process.inclusiveVertexing4 * process.cleanedInclusiveSecondaryVertexFinderTagInfosNoPu * process.cleanedCombinedInclusiveSecondaryVertexBJetTagsNoPu * process.CustombTagValidation * process.dqmSaver)

process.dqmEnv.subSystemFolder = 'BTAG'
process.dqmSaver.producer = 'DQM'
process.dqmSaver.workflow = '/POG/BTAG/BJETALL'
process.dqmSaver.convention = 'Offline'
process.dqmSaver.saveByRun = cms.untracked.int32(-1)
process.dqmSaver.saveAtJobEnd =cms.untracked.bool(True) 
process.dqmSaver.forceRunNumber = cms.untracked.int32(1)
#process.PoolSource.fileNames = [
##'file:/networkdata/arizzi/QCD470600/00256BFE-F307-E211-A264-003048FFCB96.root',
##'file:/networkdata/arizzi/QCD470600/00490DE0-DD07-E211-B197-001A928116B2.root',
##'file:/networkdata/arizzi/QCD470600/00755239-F507-E211-A7BD-00261894393F.root'
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
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/04662905-1C12-E211-AA36-848F69FD2823.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/04662F83-E712-E211-BF15-00266CF268B8.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/046D1088-9B12-E211-838D-008CFA000744.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/04827982-A611-E211-B849-848F69FD28C8.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/04E17911-FF10-E211-9867-00266CF9B04C.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/04E70BEC-1212-E211-81A8-00A0D1EE25D0.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/04FA6F4E-DF11-E211-B7D1-848F69FD5027.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/06410C50-BC11-E211-B67A-00266CFAEA68.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/06483FE2-FF10-E211-AF0D-848F69FD45A7.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/069F36FB-0F12-E211-BD08-848F69FD4E14.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/069FBF82-8810-E211-A84E-00266CF9B04C.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/06ADC0D0-7511-E211-BBFC-00266CFAE69C.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/06C6F35C-FC11-E211-A03D-00266CF9AD20.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/08284FEF-DC11-E211-BBBD-0024E8769965.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0842AA8F-CB12-E211-A5BA-0024E876A7C6.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0854B17B-5312-E211-B293-008CFA0021D4.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0856427C-4311-E211-BB1F-0026B94D1AD5.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/08A2FD23-2111-E211-AE50-008CFA002490.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/08A5558B-2612-E211-8AFF-00A0D1EEF5B4.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/08ABCBEA-C212-E211-9E2C-0024E8769B39.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/08C53758-AC12-E211-930A-00266CF9B684.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/08D60329-0A11-E211-88BB-848F69FD2520.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0A689B53-AE11-E211-A719-848F69FD289B.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0A862B6E-AD12-E211-AC79-848F69FD25BC.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0AAF585D-1812-E211-BA12-848F69FD289B.root",
#"/store/mc/Summer12_DR53X/QCD_HT-250To500_TuneZ2star_8TeV-madgraph-pythia6/AODSIM/PU_S10_START53_V7A-v1/00000/0ABF8933-C511-E211-BB61-0026B94D1AEF.root"
#]



