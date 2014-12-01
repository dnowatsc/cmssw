# The following comments couldn't be translated into the new config version:

#! /bin/env cmsRun

#file used in the past to run on RECO files, now used for harvesting only on DQM files
import FWCore.ParameterSet.Config as cms


process = cms.Process("analyzeNI")

#keep the logging output to a nice level
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.Geometry.GeometryIdeal_cff")
#process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.GlobalTag.globaltag = "POSTLS172_V3::All"

process.load("RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff")
process.load("RecoBTag.SecondaryVertex.nuclearInteractionIdentifier_cfi")

process.niId = process.nuclearInteractionCandIdentifier.clone(
	selection = cms.PSet(
		maxZ = cms.double(29.),				# maximum Z of SV to be identified as NI
		position = cms.vdouble(3.52, 5.11, 6.64, 8.01, 9.53, 10.64),			# vector of doubles, even entries setting the lower limits for NI id, odd entries the upper limits
		#minTrack3DipSig = cms.double(0.),
		# minMass = cms.double()			# minimum and maximum vertex mass for NI id
		# maxMass = cms.double()			
		# minNtracks = cms.int32()			# minimum and maximum vertex number of daughter for NI id
		# maxNtracks = cms.int32()
		#minNctau = cms.double(0.)			# minimum Nctau = flightDistance2D/(pt/mass*0.05) for NI id (checking for compatibility with B hadron flight length)
		# distToNI = cms.double()			# if another SV (not flagged as NI) is closer than distToNI to a NI, it will also be flagged as one
		)
	
)

process.niAnalyzer = cms.EDAnalyzer("NIAnalyzer",
	secondaryVertices = cms.InputTag("niId"),
	primaryVertices = cms.InputTag("offlinePrimaryVertices"),
	beamSpot = cms.InputTag("offlineBeamSpot")
)


process.path = cms.Path(
    process.inclusiveCandidateVertexing
	* process.niId
	* process.niAnalyzer
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(301)
)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring()
)



process.PoolSource.fileNames = [
	'/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/00753DE2-C730-E411-957F-0025905B85EE.root',
	'/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/2C59AFBB-DA30-E411-832E-002590596498.root',
	'/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/80BE6D20-DB30-E411-9644-0025905A609A.root',
	'/store/relval/CMSSW_7_2_0_pre5/RelValProdQCD_Pt_3000_3500_13/GEN-SIM-RECO/POSTLS172_V3-v1/00000/BA1A0B52-C630-E411-B8D5-002618943896.root',
	#'/store/relval/CMSSW_7_2_0_pre1/RelValTTbar_13/GEN-SIM-RECO/POSTLS172_V1-v1/00000/0C41D097-EEFD-E311-9E85-0025905A6066.root',
	#'/store/relval/CMSSW_7_2_0_pre1/RelValTTbar_13/GEN-SIM-RECO/POSTLS172_V1-v1/00000/26C5E596-F0FD-E311-A15D-0025905B8596.root',
	#'/store/relval/CMSSW_7_2_0_pre1/RelValTTbar_13/GEN-SIM-RECO/POSTLS172_V1-v1/00000/8C29EAA9-F0FD-E311-B63D-003048FFD736.root'
]

