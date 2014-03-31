import FWCore.ParameterSet.Config as cms


nuclearInteractionFinderMod = cms.EDAnalyzer("NuclearInteractionIdentifierDetector",
	primaryVertices = cms.InputTag("offlinePrimaryVertices"),
	secondaryVertices = cms.InputTag("inclusiveMergedVertices"),
	beamSpot = cms.InputTag("offlineBeamSpot")
)
