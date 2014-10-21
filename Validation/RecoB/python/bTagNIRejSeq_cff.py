import FWCore.ParameterSet.Config as cms

# standard CSVIVFv2 sequence with removed position cut on SV in pfInclusiveSecondaryVertexFinderTagInfos

from RecoBTag.SoftLepton.softLepton_cff import *
from RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff import *
from RecoBTag.SecondaryVertex.pfInclusiveSecondaryVertexFinderTagInfos_cfi import *
from RecoBTag.ImpactParameter.pfImpactParameter_cfi import *

from RecoBTag.SecondaryVertex.candidateCombinedSecondaryVertexES_cfi import *
from RecoBTag.SecondaryVertex.pfCombinedSecondaryVertexBJetTags_cfi import *
from RecoBTag.SecondaryVertex.combinedInclusiveSecondaryVertexBJetTags_cfi import *


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


# sequence with NIs removed


from RecoBTag.SecondaryVertex.nuclearInteractionIdentification_cff import *

pfImpactParameterTagInfosCleaned = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleanedWithCuts")
)

pfInclusiveSecondaryVertexFinderTagInfosCleaned = pfInclusiveSecondaryVertexFinderTagInfosNoMin2D.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleaned"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCandAfterNI")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned = pfCombinedInclusiveSecondaryVertexV2BJetTagsNoMin2D.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleaned"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleaned"))
)

# entire sequence

MYbtagSequenceNIremovedWithCuts = cms.Sequence(
	inclusiveCandidateVertexing *
	nuclearInteractionsRemovedWithCuts *
	pfImpactParameterTagInfosCleaned *
	pfInclusiveSecondaryVertexFinderTagInfosCleaned *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned *
	softPFElectronsTagInfos
)
