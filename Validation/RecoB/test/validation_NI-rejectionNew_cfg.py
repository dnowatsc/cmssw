#Basic example configration file to run the b-tagging validation sequence.
import FWCore.ParameterSet.Config as cms
process = cms.Process("validation")

"""
start customization
"""

#Enter here the Global tags
tag =  'POSTLS172_V3::All'
#Flavour plots for MC: "all" = plots for all jets ; "dusg" = plots for d, u, s, dus, g independently ; not mandatory and any combinations are possible 
#b, c, light (dusg), non-identified (NI), PU jets plots are always produced
flavPlots = "allbcldusg"
ptRanges = cms.vdouble(50.0, 150.0, 500., 1500., 3000.0)
#Check if jets originate from PU? option recommended (only for MC)
PUid = True
#List of taggers and taginfo to be considered (see example in: DQMOffline/RecoB/python/bTagCommon_cff.py)
from DQMOffline.RecoB.bTagCommon_cff import *
from DQMOffline.RecoB.bTagCombinedSVAnalysisNew_cff import *	# with larger x-ranges for some vertexFlightDistance plots

tagConfig = cms.VPSet(
        cms.PSet(
            bTagGenericAnalysisBlock,
            label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTags"),
            folder = cms.string("CSVIVFv2-StandardRho25")	# standard CSVIVFv2 sequence
        ),
        cms.PSet(
            bTagCombinedSVAnalysisBlockNew,
            ipTagInfos = cms.InputTag("pfImpactParameterTagInfos"),
            type = cms.string('GenericMVA'),
            svTagInfos = cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfos"),
            label = cms.InputTag("candidateCombinedSecondaryVertexV2"),
            folder = cms.string("CSVIVFv2-StandardRho25")
        ),
        #cms.PSet(
            #bTagGenericAnalysisBlock,
            #label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTagsRho90"),
            #folder = cms.string("CSVIVFv2-StandardRho90")	# standard CSVIVFv2 sequence
        #),
        cms.PSet(
            bTagGenericAnalysisBlock,
            label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTagsRho9999"),
            folder = cms.string("CSVIVFv2-StandardRho9999")	# standard CSVIVFv2 sequence
        ),
        cms.PSet(
            bTagCombinedSVAnalysisBlockNew,
            ipTagInfos = cms.InputTag("pfImpactParameterTagInfos"),
            type = cms.string('GenericMVA'),
            svTagInfos = cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosRho9999"),
            label = cms.InputTag("candidateCombinedSecondaryVertexV2"),
            folder = cms.string("CSVIVFv2-StandardRho9999")
        ),
)

from RecoBTag.SecondaryVertex.nuclearInteractionIdentificationNew_cff import Nversion2
from RecoBTag.SecondaryVertex.nuclearInteractionIdentificationNew_cff import rhoCuts

for rho in rhoCuts :
	for i in range(0, Nversion2) :
		tagConfig.append(
			cms.PSet(
				bTagGenericAnalysisBlock,
				label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned"+rho+"v%s"%i),
				folder = cms.string("CSVIVFv2-NICleaned"+rho+"v%s"%i)	# standard CSVIVFv2 sequence
			)
		)
		tagConfig.append(
			cms.PSet(
				bTagCombinedSVAnalysisBlockNew,
				ipTagInfos = cms.InputTag('pfImpactParameterTagInfosCleaned'+rho+'v%s'%i),
				type = cms.string('GenericMVA'),
				svTagInfos = cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleaned"+rho+"v%s"%i),
				label = cms.InputTag("candidateCombinedSecondaryVertexV2"),
				folder = cms.string("CSVIVFv2-NICleaned"+rho+"v%s"%i)
			)
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

#process.load('CondCore.DBCommon.CondDBSetup_cfi')
#process.BTauMVAJetTagComputerRecord = cms.ESSource('PoolDBESSource',
    #process.CondDBSetup,
    #timetype = cms.string('runnumber'),
    #toGet = cms.VPSet(cms.PSet(
        #record = cms.string('BTauGenericMVAJetTagComputerRcd'),
        #tag = cms.string('MVAComputerContainer_53X_JetTags_v3')
    #)),
    #connect = cms.string('frontier://FrontierProd/CMS_COND_PAT_000'),
    #BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
#)
#process.es_prefer_BTauMVAJetTagComputerRecord = cms.ESPrefer('PoolDBESSource','BTauMVAJetTagComputerRecord')

#=============

#Validation sequence
process.load("Validation.RecoB.bTagAnalysis_cfi")
process.bTagValidation.applyPtHatWeight = True
process.bTagValidation.jetMCSrc = 'AK4byValAlgo'
process.bTagValidation.tagConfig = tagConfig
process.bTagHarvestMC.tagConfig = tagConfig
process.bTagValidation.flavPlots = flavPlots
process.bTagHarvestMC.flavPlots = flavPlots
process.bTagValidation.ptRanges = ptRanges
process.bTagHarvestMC.ptRanges = ptRanges
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

#process.load("Validation.RecoB.bTagNIRejSeqRhoCut25_cff")
#process.load("Validation.RecoB.bTagNIRejSeqRhoCut90_cff")
#process.load("Validation.RecoB.bTagNIRejSeqRhoCut9999_cff")

process.load("Validation.RecoB.bTagNIRejSeq_cff")

#process.bTagSeq = cms.Sequence(
	#process.niRejSeqRhoCut25 *
	#process.niRejSeqRhoCut90 *
	#process.niRejSeqRhoCut9999
#)

process.dqmSeq = cms.Sequence(process.ak4GenJetsForPUid * process.patJetGenJetMatch * process.flavourSeq * process.bTagValidation * process.bTagHarvestMC * process.dqmSaver)

process.plots = cms.Path(process.bTagSeq * process.dqmSeq)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

version = '02_9000_noWeight_rho25_rho9999_QCD_3000-3500_vertReFit_v0-3'
    
process.dqmEnv.subSystemFolder = 'BTAG'
process.dqmSaver.producer = 'DQM'
process.dqmSaver.workflow = '/POG/BTAG/BJET_%s'%version
process.dqmSaver.convention = 'Offline'
process.dqmSaver.saveByRun = cms.untracked.int32(-1)
process.dqmSaver.saveAtJobEnd =cms.untracked.bool(True) 
process.dqmSaver.forceRunNumber = cms.untracked.int32(1)
process.PoolSource.fileNames = [
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/04B42DBE-9F30-E411-94AD-002618FDA208.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/50ED7D89-7730-E411-8B92-0026189437EB.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/5EAF0D89-7730-E411-B9F2-002354EF3BE0.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/704E9FAC-8E30-E411-B25D-0025905A6090.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/88579BFC-7130-E411-AA38-0025905A60B8.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/A2CE9AFE-F230-E411-B364-0025905A60E0.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/AC2985EF-F330-E411-B3E0-0025905B855E.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/D8667945-7830-E411-9672-0025905A6090.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/EAD433E4-F330-E411-919E-0025905A60D0.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/EED73486-7730-E411-BF69-002618943894.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/FC473B93-7730-E411-BF61-0025905A6132.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_FlatPt_15_3000HS_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/FCE83CAD-6F30-E411-8986-0025905A6134.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_Pt_80_120_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/0E95A39F-7130-E411-B8F3-002618943906.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_Pt_80_120_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/8252B3FF-9830-E411-B31A-0025905B85A2.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_Pt_80_120_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/EEC0BDAC-9830-E411-A302-0025905B8582.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_Pt_600_800_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/229DB1A4-6F30-E411-9B41-0026189438CE.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_Pt_600_800_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/284C12AD-9730-E411-9C63-003048FFCC18.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_Pt_600_800_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/32254A9E-9530-E411-8520-003048FFCB6A.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValQCD_Pt_600_800_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/E22357AF-9130-E411-B493-00261894384F.root',
	'/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/00753DE2-C730-E411-957F-0025905B85EE.root',
	'/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/2C59AFBB-DA30-E411-832E-002590596498.root',
	'/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/80BE6D20-DB30-E411-9644-0025905A609A.root',
	'/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/BA1A0B52-C630-E411-B8D5-002618943896.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_1.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_10.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_11.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_12.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_13.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_14.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_15.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_16.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_17.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_18.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_2.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_3.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_4.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_5.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_6.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_7.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_8.root',
	#'/store/user/arizzi/RelValQCD_Pt_3000_3500_13/ClusterSplitting_test1/141026_190335/0000/reco-split_RAW2DIGI_RECO_9.root'
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/08C50B46-7F30-E411-80C1-0025905A6134.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/0E49EFC9-5B30-E411-8C4F-0025905A60E4.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/2C5FC4A6-5C30-E411-91E0-0026189438F5.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/304ED060-E430-E411-B99B-0025905964C2.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/C2E538F7-5C30-E411-A698-002618943862.root', 	
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/CC9B7283-8130-E411-9799-0026189438B0.root'
	#'/store/relval/CMSSW_7_2_0_pre1/RelValTTbar_13/GEN-SIM-RECO/POSTLS172_V1-v1/00000/0C41D097-EEFD-E311-9E85-0025905A6066.root',
	#'/store/relval/CMSSW_7_2_0_pre1/RelValTTbar_13/GEN-SIM-RECO/POSTLS172_V1-v1/00000/26C5E596-F0FD-E311-A15D-0025905B8596.root',
	#'/store/relval/CMSSW_7_2_0_pre1/RelValTTbar_13/GEN-SIM-RECO/POSTLS172_V1-v1/00000/8C29EAA9-F0FD-E311-B63D-003048FFD736.root'
]

