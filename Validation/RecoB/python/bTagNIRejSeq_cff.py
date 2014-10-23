import FWCore.ParameterSet.Config as cms

from RecoBTag.SoftLepton.softLepton_cff import *
from RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff import *
from RecoBTag.SecondaryVertex.pfInclusiveSecondaryVertexFinderTagInfos_cfi import *
from RecoBTag.ImpactParameter.pfImpactParameter_cfi import *

from RecoBTag.SecondaryVertex.candidateCombinedSecondaryVertexES_cfi import *
from RecoBTag.SecondaryVertex.pfCombinedSecondaryVertexBJetTags_cfi import *
from RecoBTag.SecondaryVertex.combinedInclusiveSecondaryVertexBJetTags_cfi import *

# standard CSVIVFv2 sequence

MYbtagSequenceStandard = cms.Sequence(
	inclusiveCandidateVertexing *
	pfImpactParameterTagInfos *
	pfInclusiveSecondaryVertexFinderTagInfos *
	pfCombinedInclusiveSecondaryVertexV2BJetTags *
	softPFElectronsTagInfos
)

# standard CSVIVFv2 sequence with removed position cut on SV in pfInclusiveSecondaryVertexFinderTagInfos

pfInclusiveSecondaryVertexFinderTagInfosNoMin2D = pfInclusiveSecondaryVertexFinderTagInfos.clone()
pfInclusiveSecondaryVertexFinderTagInfosNoMin2D.vertexCuts.distVal2dMax = 99999.9

pfCombinedInclusiveSecondaryVertexV2BJetTagsNoMin2D = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(
		cms.InputTag("pfImpactParameterTagInfos"),
		cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosNoMin2D")
	)
)

MYbtagSequenceNoMin2D = cms.Sequence(
	inclusiveCandidateVertexing *
	pfImpactParameterTagInfos *
	pfInclusiveSecondaryVertexFinderTagInfosNoMin2D *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsNoMin2D *
	softPFElectronsTagInfos
)




from RecoBTag.SecondaryVertex.nuclearInteractionIdentification_cff import *

# from here on up until including version 3, all NI identifiers are based on NoMin2D SVTagInfoProducer
# sequence with NIs removed (version 0, simply positions)

pfImpactParameterTagInfosCleaned0 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned0")
)

pfInclusiveSecondaryVertexFinderTagInfosCleaned0 = pfInclusiveSecondaryVertexFinderTagInfosNoMin2D.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleaned0"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned0")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned0 = pfCombinedInclusiveSecondaryVertexV2BJetTagsNoMin2D.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleaned0"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleaned0"))
)

MYbtagSequenceNIremoved0 = cms.Sequence(
	nuclearInteractionsRemoved0 *
	pfImpactParameterTagInfosCleaned0 *
	pfInclusiveSecondaryVertexFinderTagInfosCleaned0 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned0 *
	softPFElectronsTagInfos
)

# sequence with NIs removed (version 1, position + mass + ntracks cut)

pfImpactParameterTagInfosCleaned1 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned1")
)

pfInclusiveSecondaryVertexFinderTagInfosCleaned1 = pfInclusiveSecondaryVertexFinderTagInfosNoMin2D.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleaned1"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned1")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned1 = pfCombinedInclusiveSecondaryVertexV2BJetTagsNoMin2D.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleaned1"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleaned1"))
)

MYbtagSequenceNIremoved1 = cms.Sequence(
	nuclearInteractionsRemoved1 *
	pfImpactParameterTagInfosCleaned1 *
	pfInclusiveSecondaryVertexFinderTagInfosCleaned1 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned1 *
	softPFElectronsTagInfos
)

# sequence with NIs removed (version 2, position + mass + ntracks cut + NI id with relaxed IVF cuts)

pfImpactParameterTagInfosCleaned2 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned2")
)

pfInclusiveSecondaryVertexFinderTagInfosCleaned2 = pfInclusiveSecondaryVertexFinderTagInfosNoMin2D.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleaned2"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned2")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned2 = pfCombinedInclusiveSecondaryVertexV2BJetTagsNoMin2D.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleaned2"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleaned2"))
)

MYbtagSequenceNIremoved2 = cms.Sequence(
	nuclearInteractionsRemoved2 *
	pfImpactParameterTagInfosCleaned2 *
	pfInclusiveSecondaryVertexFinderTagInfosCleaned2 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned2 *
	softPFElectronsTagInfos
)

# sequence with NIs removed (version 3, only position + NI id with relaxed IVF cuts)

pfImpactParameterTagInfosCleaned3 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned3")
)

pfInclusiveSecondaryVertexFinderTagInfosCleaned3 = pfInclusiveSecondaryVertexFinderTagInfosNoMin2D.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleaned3"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned3")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned3 = pfCombinedInclusiveSecondaryVertexV2BJetTagsNoMin2D.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleaned3"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleaned3"))
)

MYbtagSequenceNIremoved3 = cms.Sequence(
	nuclearInteractionsRemoved3 *
	pfImpactParameterTagInfosCleaned3 *
	pfInclusiveSecondaryVertexFinderTagInfosCleaned3 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned3 *
	softPFElectronsTagInfos
)

# from here on, rho cut is applied again
# sequence with NIs removed, but with rho=2.5 cut (version 4, simply positions)

pfInclusiveSecondaryVertexFinderTagInfosCleaned4 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleaned0"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned0")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned4 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleaned0"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleaned4"))
)

MYbtagSequenceNIremoved4 = cms.Sequence(
	nuclearInteractionsRemoved0 *
	pfImpactParameterTagInfosCleaned0 *
	pfInclusiveSecondaryVertexFinderTagInfosCleaned4 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned4 *
	softPFElectronsTagInfos
)

# sequence with NIs removed, but with rho=2.5 cut (version 5, position + mass + ntracks cut + NI id with relaxed IVF cuts)

pfInclusiveSecondaryVertexFinderTagInfosCleaned5 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleaned2"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned2")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned5 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleaned2"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleaned5"))
)

MYbtagSequenceNIremoved5 = cms.Sequence(
	nuclearInteractionsRemoved2 *
	pfImpactParameterTagInfosCleaned2 *
	pfInclusiveSecondaryVertexFinderTagInfosCleaned5 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned5 *
	softPFElectronsTagInfos
)