import FWCore.ParameterSet.Config as cms

from RecoBTag.SecondaryVertex.nuclearInteractionIdentifier_cfi import *
from RecoBTag.SecondaryVertex.vertexAndTracksCleaner_cfi import *
from RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff import *

# IVF run with relaxed cuts to identify more NIs

inclusiveVertexFinderRelaxed = inclusiveCandidateVertexFinder.clone(
	maximumLongitudinalImpactParameter = 1.0,
	vertexMinAngleCosine = 0.8
)

vertexMergerRelaxed = candidateVertexMerger.clone( secondaryVertices = cms.InputTag("inclusiveVertexFinderRelaxed") )

trackVertexArbitratorRelaxed = candidateVertexArbitrator.clone( secondaryVertices = cms.InputTag("vertexMergerRelaxed"))

inclusiveSecondaryVerticesRelaxed = candidateVertexMerger.clone(
	secondaryVertices = cms.InputTag("trackVertexArbitratorRelaxed"),
	maxFraction = cms.double(0.2),
	minSignificance = cms.double(10.)
)

inclusiveCandidateVertexingRelaxed = cms.Sequence(inclusiveVertexFinderRelaxed*vertexMergerRelaxed*trackVertexArbitratorRelaxed*inclusiveSecondaryVerticesRelaxed)

# NI rejection and tracks cleaning procedure

# identify solely based on position
nuclearInteractionIdentifier0 = nuclearInteractionCandIdentifier.clone(
	selection = cms.PSet(
		position = cms.vdouble(2.65, 3.22, 3.52, 5.11, 6.64, 8.01, 9.53, 10.64)
	)
)

# identify based on position, mass & ntracks
nuclearInteractionIdentifier1 = nuclearInteractionCandIdentifier.clone(
	selection = cms.PSet(
		position = cms.vdouble(2.65, 3.22, 3.52, 5.11, 6.64, 8.01, 9.53, 10.64),
		maxNtracks = cms.int32(2),
		maxMass = cms.double(1.4)
	)
)

# first run IVF with relaxed cuts (maximumLongitudinalImpactParameter = 1.0, vertexMinAngleCosine = 0.8), then identify NIs based on position, mass & ntracks
nuclearInteractionIdentifier2 = nuclearInteractionIdentifier1.clone(
	secondaryVertices = cms.InputTag("inclusiveSecondaryVerticesRelaxed")
)

# as verstion 2 but again simply position-based id
nuclearInteractionIdentifier3 = nuclearInteractionIdentifier0.clone(
	secondaryVertices = cms.InputTag("inclusiveSecondaryVerticesRelaxed")
)
	
vertexAndTracksCleaned0 = vertexAndTracksCandCleaned.clone(vetoVert = "nuclearInteractionIdentifier0")
vertexAndTracksCleaned1 = vertexAndTracksCandCleaned.clone(vetoVert = "nuclearInteractionIdentifier1")
vertexAndTracksCleaned2 = vertexAndTracksCandCleaned.clone(vetoVert = "nuclearInteractionIdentifier2")
vertexAndTracksCleaned3 = vertexAndTracksCandCleaned.clone(vetoVert = "nuclearInteractionIdentifier3")

# re-run IVF

inclusiveVertexFinderCleaned0 = inclusiveCandidateVertexFinder.clone(tracks = cms.InputTag("vertexAndTracksCleaned0"))
inclusiveVertexFinderCleaned1 = inclusiveCandidateVertexFinder.clone(tracks = cms.InputTag("vertexAndTracksCleaned1"))
inclusiveVertexFinderCleaned2 = inclusiveCandidateVertexFinder.clone(tracks = cms.InputTag("vertexAndTracksCleaned2"))
inclusiveVertexFinderCleaned3 = inclusiveCandidateVertexFinder.clone(tracks = cms.InputTag("vertexAndTracksCleaned3"))

vertexMergerCleaned0 = candidateVertexMerger.clone( secondaryVertices = cms.InputTag("inclusiveVertexFinderCleaned0") )
vertexMergerCleaned1 = candidateVertexMerger.clone( secondaryVertices = cms.InputTag("inclusiveVertexFinderCleaned1") )
vertexMergerCleaned2 = candidateVertexMerger.clone( secondaryVertices = cms.InputTag("inclusiveVertexFinderCleaned2") )
vertexMergerCleaned3 = candidateVertexMerger.clone( secondaryVertices = cms.InputTag("inclusiveVertexFinderCleaned3") )

trackVertexArbitratorCleaned0 = candidateVertexArbitrator.clone(tracks = cms.InputTag("vertexAndTracksCleaned0"), secondaryVertices = cms.InputTag("vertexMergerCleaned0"))
trackVertexArbitratorCleaned1 = candidateVertexArbitrator.clone(tracks = cms.InputTag("vertexAndTracksCleaned1"), secondaryVertices = cms.InputTag("vertexMergerCleaned1"))
trackVertexArbitratorCleaned2 = candidateVertexArbitrator.clone(tracks = cms.InputTag("vertexAndTracksCleaned2"), secondaryVertices = cms.InputTag("vertexMergerCleaned2"))
trackVertexArbitratorCleaned3 = candidateVertexArbitrator.clone(tracks = cms.InputTag("vertexAndTracksCleaned3"), secondaryVertices = cms.InputTag("vertexMergerCleaned3"))

inclusiveSecondaryVerticesCleaned0 = candidateVertexMerger.clone(
	secondaryVertices = cms.InputTag("trackVertexArbitratorCleaned0"),
	maxFraction = cms.double(0.2),
	minSignificance = cms.double(10.)
)

inclusiveSecondaryVerticesCleaned1 = inclusiveSecondaryVerticesCleaned0.clone(secondaryVertices = cms.InputTag("trackVertexArbitratorCleaned1"))
inclusiveSecondaryVerticesCleaned2 = inclusiveSecondaryVerticesCleaned0.clone(secondaryVertices = cms.InputTag("trackVertexArbitratorCleaned2"))
inclusiveSecondaryVerticesCleaned3 = inclusiveSecondaryVerticesCleaned0.clone(secondaryVertices = cms.InputTag("trackVertexArbitratorCleaned3"))

nuclearInteractionsRemoved0 = cms.Sequence(
	inclusiveCandidateVertexing *
	nuclearInteractionIdentifier0 *
	vertexAndTracksCleaned0 *
	inclusiveVertexFinderCleaned0 *
	vertexMergerCleaned0 *
	trackVertexArbitratorCleaned0 *
	inclusiveSecondaryVerticesCleaned0
)

nuclearInteractionsRemoved1 = cms.Sequence(
	inclusiveCandidateVertexing *
	nuclearInteractionIdentifier1 *
	vertexAndTracksCleaned1 *
	inclusiveVertexFinderCleaned1 *
	vertexMergerCleaned1 *
	trackVertexArbitratorCleaned1 *
	inclusiveSecondaryVerticesCleaned1
)

nuclearInteractionsRemoved2 = cms.Sequence(
	inclusiveCandidateVertexingRelaxed *
	nuclearInteractionIdentifier2 *
	vertexAndTracksCleaned2 *
	inclusiveVertexFinderCleaned2 *
	vertexMergerCleaned2 *
	trackVertexArbitratorCleaned2 *
	inclusiveSecondaryVerticesCleaned2
)

nuclearInteractionsRemoved3 = cms.Sequence(
	inclusiveCandidateVertexingRelaxed *
	nuclearInteractionIdentifier3 *
	vertexAndTracksCleaned3 *
	inclusiveVertexFinderCleaned3 *
	vertexMergerCleaned3 *
	trackVertexArbitratorCleaned3 *
	inclusiveSecondaryVerticesCleaned3
)