import FWCore.ParameterSet.Config as cms

from RecoBTag.SoftLepton.softLepton_cff import *
from RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff import *
from RecoBTag.SecondaryVertex.pfInclusiveSecondaryVertexFinderTagInfos_cfi import *
from RecoBTag.ImpactParameter.pfImpactParameter_cfi import *

from RecoBTag.SecondaryVertex.candidateCombinedSecondaryVertexES_cfi import *
from RecoBTag.SecondaryVertex.pfCombinedSecondaryVertexBJetTags_cfi import *
from RecoBTag.SecondaryVertex.combinedInclusiveSecondaryVertexBJetTags_cfi import *


#======================================================================
# run nuclear interaction identifications with a MODIFIED
# pfInclusiveSecondaryVertexFinderTagInfos module, so removing
# the cut on the SV position
#======================================================================


# standard CSVIVFv2 sequence with removed rho cut in pfInclusiveSecondaryVertexFinderTagInfos

pfInclusiveSecondaryVertexFinderTagInfosRho9999 = pfInclusiveSecondaryVertexFinderTagInfos.clone()
pfInclusiveSecondaryVertexFinderTagInfosRho9999.vertexCuts.distVal2dMax = 9999.9

pfCombinedInclusiveSecondaryVertexV2BJetTagsRho9999 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(
		cms.InputTag("pfImpactParameterTagInfos"),
		cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosRho9999")
	)
)

MYbtagSequenceStandardRho9999 = cms.Sequence(
	inclusiveCandidateVertexing *
	pfImpactParameterTagInfos *
	pfInclusiveSecondaryVertexFinderTagInfosRho9999 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsRho9999 *
	softPFElectronsTagInfos
)




from RecoBTag.SecondaryVertex.nuclearInteractionIdentification_cff import *

# NI rejection version 0

pfImpactParameterTagInfosCleanedRho9999v0 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned0")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v0 = pfInclusiveSecondaryVertexFinderTagInfosRho9999.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v0"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned0")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v0 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v0"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v0"))
)

MYbtagSequenceNIremovedRho9999v0 = cms.Sequence(
	nuclearInteractionsRemoved0 *
	pfImpactParameterTagInfosCleanedRho9999v0 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v0 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v0 *
	softPFElectronsTagInfos
)

# NI rejection version 1

pfImpactParameterTagInfosCleanedRho9999v1 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned1")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v1 = pfInclusiveSecondaryVertexFinderTagInfosRho9999.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v1"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned1")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v1 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v1"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v1"))
)

MYbtagSequenceNIremovedRho9999v1 = cms.Sequence(
	nuclearInteractionsRemoved1 *
	pfImpactParameterTagInfosCleanedRho9999v1 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v1 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v1 *
	softPFElectronsTagInfos
)

# NI rejection version 2

pfImpactParameterTagInfosCleanedRho9999v2 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned2")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v2 = pfInclusiveSecondaryVertexFinderTagInfosRho9999.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v2"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned2")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v2 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v2"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v2"))
)

MYbtagSequenceNIremovedRho9999v2 = cms.Sequence(
	nuclearInteractionsRemoved2 *
	pfImpactParameterTagInfosCleanedRho9999v2 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v2 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v2 *
	softPFElectronsTagInfos
)

# NI rejection version 3

pfImpactParameterTagInfosCleanedRho9999v3 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned3")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v3 = pfInclusiveSecondaryVertexFinderTagInfosRho9999.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v3"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned3")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v3 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v3"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v3"))
)

MYbtagSequenceNIremovedRho9999v3 = cms.Sequence(
	nuclearInteractionsRemoved3 *
	pfImpactParameterTagInfosCleanedRho9999v3 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v3 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v3 *
	softPFElectronsTagInfos
)

# NI rejection version 4

pfImpactParameterTagInfosCleanedRho9999v4 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned4")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v4 = pfInclusiveSecondaryVertexFinderTagInfosRho9999.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v4"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned4")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v4 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v4"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v4"))
)

MYbtagSequenceNIremovedRho9999v4 = cms.Sequence(
	nuclearInteractionsRemoved4 *
	pfImpactParameterTagInfosCleanedRho9999v4 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v4 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v4 *
	softPFElectronsTagInfos
)

# NI rejection version 5

pfImpactParameterTagInfosCleanedRho9999v5 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned5")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v5 = pfInclusiveSecondaryVertexFinderTagInfosRho9999.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v5"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned5")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v5 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v5"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v5"))
)

MYbtagSequenceNIremovedRho9999v5 = cms.Sequence(
	nuclearInteractionsRemoved5 *
	pfImpactParameterTagInfosCleanedRho9999v5 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v5 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v5 *
	softPFElectronsTagInfos
)

# NI rejection version 6

pfImpactParameterTagInfosCleanedRho9999v6 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned6")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v6 = pfInclusiveSecondaryVertexFinderTagInfosRho9999.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v6"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned6")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v6 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v6"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v6"))
)

MYbtagSequenceNIremovedRho9999v6 = cms.Sequence(
	nuclearInteractionsRemoved6 *
	pfImpactParameterTagInfosCleanedRho9999v6 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v6 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v6 *
	softPFElectronsTagInfos
)

# NI rejection version 7

pfImpactParameterTagInfosCleanedRho9999v7 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned7")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v7 = pfInclusiveSecondaryVertexFinderTagInfosRho9999.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v7"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned7")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v7 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho9999v7"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v7"))
)

MYbtagSequenceNIremovedRho9999v7 = cms.Sequence(
	nuclearInteractionsRemoved7 *
	pfImpactParameterTagInfosCleanedRho9999v7 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho9999v7 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho9999v7 *
	softPFElectronsTagInfos
)

# all b-tagging sequences

niRejSeqRhoCut9999 = cms.Sequence(
	MYbtagSequenceStandardRho9999 *
	MYbtagSequenceNIremovedRho9999v0 *
	MYbtagSequenceNIremovedRho9999v1 *
	MYbtagSequenceNIremovedRho9999v2 *
	MYbtagSequenceNIremovedRho9999v3 *
	MYbtagSequenceNIremovedRho9999v4 *
	MYbtagSequenceNIremovedRho9999v5 *
	MYbtagSequenceNIremovedRho9999v6 *
	MYbtagSequenceNIremovedRho9999v7
)