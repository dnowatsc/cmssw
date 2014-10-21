import FWCore.ParameterSet.Config as cms

from RecoBTag.SecondaryVertex.nuclearInteractionIdentifier_cfi import *
from RecoBTag.SecondaryVertex.vertexAndTracksCleaner_cfi import *
from RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff import *

nuclearInteractionIdentifierWithCuts = nuclearInteractionCandIdentifier.clone(
	selection = cms.PSet(
		position = cms.vdouble(2.65, 3.22, 3.52, 5.11, 6.64, 8.01, 9.53, 10.64),
		maxNtracks = cms.int32(2),
		maxMass = cms.double(1.4)
	)
)

vertexAndTracksCleanedWithCuts = vertexAndTracksCandCleaned.clone(
	vetoVert = "nuclearInteractionIdentifierWithCuts"
)

# re-run IVF

inclusiveVertexFinderCandAfterNI = inclusiveCandidateVertexFinder.clone(
	#primaryVertices = cms.InputTag("offlinePrimaryVerticesNewPrimVert"),
	tracks = cms.InputTag("vertexAndTracksCleanedWithCuts")
)

vertexMergerCandAfterNI = candidateVertexMerger.clone( secondaryVertices = cms.InputTag("inclusiveVertexFinderCandAfterNI") )

trackVertexArbitratorCandAfterNI = candidateVertexArbitrator.clone(
	#primaryVertices = cms.InputTag("offlinePrimaryVerticesNewPrimVert"),
	tracks = cms.InputTag("vertexAndTracksCleanedWithCuts"),
	secondaryVertices = cms.InputTag("vertexMergerCandAfterNI")
)

inclusiveSecondaryVerticesCandAfterNI = candidateVertexMerger.clone(
	secondaryVertices = cms.InputTag("trackVertexArbitratorCandAfterNI"),
	maxFraction = cms.double(0.2),
	minSignificance = cms.double(10.)
)

nuclearInteractionsRemovedWithCuts = cms.Sequence(
	nuclearInteractionIdentifierWithCuts *
	vertexAndTracksCleanedWithCuts *
	inclusiveVertexFinderCandAfterNI *
	vertexMergerCandAfterNI *
	trackVertexArbitratorCandAfterNI *
	inclusiveSecondaryVerticesCandAfterNI
	)