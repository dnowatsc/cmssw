import FWCore.ParameterSet.Config as cms

vertexAndTracksCandCleaned = cms.EDProducer("VertexCandidateCleaner",
      vetoVert  = cms.InputTag("nuclearInteractionCandIdentifier"),
      #secondaryVertices = cms.InputTag("inclusiveCandidateSecondaryVertices"),
      srcTracks = cms.InputTag("particleFlow")
)
