import FWCore.ParameterSet.Config as cms



nuclearInteractionCandIdentifier = cms.EDProducer("NuclearInteractionCandidateIdentifier",
      primaryVertices  = cms.InputTag("offlinePrimaryVertices"),
      secondaryVertices = cms.InputTag("inclusiveCandidateSecondaryVertices"),
      selection = cms.PSet( maxZ = cms.double(29.) )
)