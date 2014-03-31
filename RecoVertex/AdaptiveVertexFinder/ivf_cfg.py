import FWCore.ParameterSet.Config as cms

process = cms.Process("validation")

process.load("RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")

from Configuration.AlCa.autoCond import autoCond 
process.GlobalTag.globaltag = autoCond['startup']

process.maxEvents = cms.untracked.PSet(
	input = cms.untracked.int32(100)
)

process.out = cms.OutputModule("PoolOutputModule",
	outputCommands = cms.untracked.vstring('keep *'), 
	fileName = cms.untracked.string('outfile.root')
)

process.path = cms.Path(
	process.inclusiveVertexing
)

process.output = cms.EndPath(
	process.out
)

process.source = cms.Source("PoolSource",
fileNames = cms.untracked.vstring(
'/store/mc/Summer12_DR53X/TT_CT10_TuneZ2star_8TeV-powheg-tauola/AODSIM/PU_S10_START53_V7A-v2/0000/00052A2F-9901-E211-A26E-001A928116D6.root'
	)
)