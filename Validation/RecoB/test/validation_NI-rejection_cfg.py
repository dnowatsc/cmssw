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
ptRanges = cms.vdouble(50.0, 150.0, 500.0, 3000.0)
#Check if jets originate from PU? option recommended (only for MC)
PUid = True
#List of taggers and taginfo to be considered (see example in: DQMOffline/RecoB/python/bTagCommon_cff.py)
from DQMOffline.RecoB.bTagCommon_cff import *
tagConfig = cms.VPSet(
        cms.PSet(
            bTagGenericAnalysisBlock,
            label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTags"),
            folder = cms.string("CSVIVFv2-Standard")	# standard CSVIVFv2 sequence
        ),
		cms.PSet(
            bTagGenericAnalysisBlock,
            label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTagsNoMin2D"),
            folder = cms.string("CSVIVFv2-NoMin2D")		# standard CSVIVFv2 sequence with removed position cut on SV in pfInclusiveSecondaryVertexFinderTagInfos
        ),
        cms.PSet(
            bTagGenericAnalysisBlock,
            label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned0"),
            folder = cms.string("CSVIVFv2-NICleaned0")	# sequence with NIs removed (version 0, simply positions)
        ),
		cms.PSet(
            bTagGenericAnalysisBlock,
            label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned1"),
            folder = cms.string("CSVIVFv2-NICleaned1")	# sequence with NIs removed (version 1, position + mass + ntracks cut)
        ),
		cms.PSet(
            bTagGenericAnalysisBlock,
            label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned2"),
            folder = cms.string("CSVIVFv2-NICleaned2")	# sequence with NIs removed (version 2, position + mass + ntracks cut + NI id with relaxed IVF cuts)
        ),
		cms.PSet(
            bTagGenericAnalysisBlock,
            label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned3"),
            folder = cms.string("CSVIVFv2-NICleaned3")	# sequence with NIs removed (version 3, only position + NI id with relaxed IVF cuts)
        ),
		cms.PSet(
            bTagGenericAnalysisBlock,
            label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned4"),
            folder = cms.string("CSVIVFv2-NICleaned4")	# sequence with NIs removed, but with rho=2.5 cut (version 4, simply positions)
        ),
		cms.PSet(
            bTagGenericAnalysisBlock,
            label = cms.InputTag("pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned5"),
            folder = cms.string("CSVIVFv2-NICleaned5")	# sequence with NIs removed, but with rho=2.5 cut (version 5, position + mass + ntracks cut + NI id with relaxed IVF cuts)
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

process.load("Validation.RecoB.bTagNIRejSeq_cff")

process.bTagSeq = cms.Sequence(
	process.MYbtagSequenceStandard *	# standard CSVIVFv2 sequence
	process.MYbtagSequenceNoMin2D *		# standard CSVIVFv2 sequence with removed position cut on SV in pfInclusiveSecondaryVertexFinderTagInfos
	process.MYbtagSequenceNIremoved0 *	# sequence with NIs removed (version 0, simply positions)
	process.MYbtagSequenceNIremoved1 *	# sequence with NIs removed (version 1, position + mass + ntracks cut)
	process.MYbtagSequenceNIremoved2 *	# sequence with NIs removed (version 2, position + mass + ntracks cut + NI id with relaxed IVF cuts)
	process.MYbtagSequenceNIremoved3 *	# sequence with NIs removed (version 3, only position + NI id with relaxed IVF cuts)
	process.MYbtagSequenceNIremoved4 *	# sequence with NIs removed, but with rho=2.5 cut (version 4, simply positions)
	process.MYbtagSequenceNIremoved5	# sequence with NIs removed, but with rho=2.5 cut (version 5, position + mass + ntracks cut + NI id with relaxed IVF cuts)
	
)

process.dqmSeq = cms.Sequence(process.ak4GenJetsForPUid * process.patJetGenJetMatch * process.flavourSeq * process.bTagValidation * process.bTagHarvestMC * process.dqmSaver)

process.plots = cms.Path(process.bTagSeq * process.dqmSeq)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

version = '03_9000ev_stand_nomin2d_nirej012345'
    
process.dqmEnv.subSystemFolder = 'BTAG'
process.dqmSaver.producer = 'DQM'
process.dqmSaver.workflow = '/POG/BTAG/BJET_%s'%version
process.dqmSaver.convention = 'Offline'
process.dqmSaver.saveByRun = cms.untracked.int32(-1)
process.dqmSaver.saveAtJobEnd =cms.untracked.bool(True) 
process.dqmSaver.forceRunNumber = cms.untracked.int32(1)
process.PoolSource.fileNames = [
	'/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/00753DE2-C730-E411-957F-0025905B85EE.root', '/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/00753DE2-C730-E411-957F-0025905B85EE.root',
	'/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/2C59AFBB-DA30-E411-832E-002590596498.root', '/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/2C59AFBB-DA30-E411-832E-002590596498.root',
	'/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/80BE6D20-DB30-E411-9644-0025905A609A.root', '/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/80BE6D20-DB30-E411-9644-0025905A609A.root',
	'/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/BA1A0B52-C630-E411-B8D5-002618943896.root', '/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/BA1A0B52-C630-E411-B8D5-002618943896.root'
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/08C50B46-7F30-E411-80C1-0025905A6134.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/08C50B46-7F30-E411-80C1-0025905A6134.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/0E49EFC9-5B30-E411-8C4F-0025905A60E4.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/0E49EFC9-5B30-E411-8C4F-0025905A60E4.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/2C5FC4A6-5C30-E411-91E0-0026189438F5.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/2C5FC4A6-5C30-E411-91E0-0026189438F5.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/304ED060-E430-E411-B99B-0025905964C2.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/304ED060-E430-E411-B99B-0025905964C2.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/C2E538F7-5C30-E411-A698-002618943862.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/C2E538F7-5C30-E411-A698-002618943862.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/CC9B7283-8130-E411-9799-0026189438B0.root',
	#'/store/relval/CMSSW_7_2_0_pre5/RelValTTbar_13/GEN-SIM-RECO/PU25ns_POSTLS172_V3-v1/00000/CC9B7283-8130-E411-9799-0026189438B0.root'
	#'/store/relval/CMSSW_7_2_0_pre1/RelValTTbar_13/GEN-SIM-RECO/POSTLS172_V1-v1/00000/0C41D097-EEFD-E311-9E85-0025905A6066.root',
	#'/store/relval/CMSSW_7_2_0_pre1/RelValTTbar_13/GEN-SIM-RECO/POSTLS172_V1-v1/00000/26C5E596-F0FD-E311-A15D-0025905B8596.root',
	#'/store/relval/CMSSW_7_2_0_pre1/RelValTTbar_13/GEN-SIM-RECO/POSTLS172_V1-v1/00000/8C29EAA9-F0FD-E311-B63D-003048FFD736.root'
]

