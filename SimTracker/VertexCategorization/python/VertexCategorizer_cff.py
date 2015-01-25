import FWCore.ParameterSet.Config as cms

from SimGeneral.HepPDTESSource.pythiapdt_cfi import *

from SimTracker.TrackAssociation.quickTrackAssociatorByHits_cfi import *

#from SimTracker.TrackHistory.VertexHistory_cff import *

vertexCategorizer = cms.PSet(
	bestMatchByMaxValue = cms.untracked.bool(True),
    beamSpot = cms.untracked.InputTag("offlineBeamSpot"),
	trackingTruth = cms.untracked.InputTag("mix","MergedTrackTruth"),
	candidates = cms.untracked.InputTag("particleFlow"),
	trackAssociator = cms.untracked.string('quickTrackAssociatorByHits'),
	trackProducer = cms.untracked.InputTag("generalTracks"),
	vertexProducer = cms.untracked.InputTag('offlinePrimaryVertices'),
	enableRecoToSim = cms.untracked.bool(True),
	enableSimToReco = cms.untracked.bool(False),
    hepMC = cms.untracked.InputTag("generator"),
    longLivedDecayLength = cms.untracked.double(1e-14),
    vertexClusteringDistance = cms.untracked.double(0.003)
)