import FWCore.ParameterSet.Config as cms

from RecoBTag.SoftLepton.softLepton_cff import *
from RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff import *
from RecoBTag.SecondaryVertex.pfInclusiveSecondaryVertexFinderTagInfos_cfi import *
from RecoBTag.ImpactParameter.pfImpactParameter_cfi import *

from RecoBTag.SecondaryVertex.candidateCombinedSecondaryVertexES_cfi import *
from RecoBTag.SecondaryVertex.pfCombinedSecondaryVertexBJetTags_cfi import *
from RecoBTag.SecondaryVertex.combinedInclusiveSecondaryVertexBJetTags_cfi import *

# standard CSVIVFv2 sequence

MYbtagSequenceStandardRho25 = cms.Sequence(
	inclusiveCandidateVertexing *
	pfImpactParameterTagInfos *
	pfInclusiveSecondaryVertexFinderTagInfos *
	pfCombinedInclusiveSecondaryVertexV2BJetTags *
	softPFElectronsTagInfos
)

## standard CSVIVFv2 sequence with removed position cut on SV in pfInclusiveSecondaryVertexFinderTagInfos

#pfInclusiveSecondaryVertexFinderTagInfosNoMin2D = pfInclusiveSecondaryVertexFinderTagInfos.clone()
#pfInclusiveSecondaryVertexFinderTagInfosNoMin2D.vertexCuts.distVal2dMax = 99999.9

#pfCombinedInclusiveSecondaryVertexV2BJetTagsNoMin2D = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	#tagInfos = cms.VInputTag(
		#cms.InputTag("pfImpactParameterTagInfos"),
		#cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosNoMin2D")
	#)
#)

#MYbtagSequenceNoMin2D = cms.Sequence(
	#inclusiveCandidateVertexing *
	#pfImpactParameterTagInfos *
	#pfInclusiveSecondaryVertexFinderTagInfosNoMin2D *
	#pfCombinedInclusiveSecondaryVertexV2BJetTagsNoMin2D *
	#softPFElectronsTagInfos
#)




from RecoBTag.SecondaryVertex.nuclearInteractionIdentification_cff import *

# from here on up until including version 3, all NI identifiers are based on NoMin2D SVTagInfoProducer
# sequence with NIs removed (version 0, simply positions)

pfImpactParameterTagInfosCleanedRho25v0 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned0")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v0 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v0"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned0")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v0 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v0"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v0"))
)

MYbtagSequenceNIremovedRho25v0 = cms.Sequence(
	nuclearInteractionsRemoved0 *
	pfImpactParameterTagInfosCleanedRho25v0 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v0 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v0 *
	softPFElectronsTagInfos
)

# sequence with NIs removed (version 1, position + mass + ntracks cut)

pfImpactParameterTagInfosCleanedRho25v1 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned1")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v1 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v1"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned1")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v1 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v1"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v1"))
)

MYbtagSequenceNIremovedRho25v1 = cms.Sequence(
	nuclearInteractionsRemoved1 *
	pfImpactParameterTagInfosCleanedRho25v1 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v1 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v1 *
	softPFElectronsTagInfos
)

# sequence with NIs removed (version 2, position + mass + ntracks cut + NI id with relaxed IVF cuts)

pfImpactParameterTagInfosCleanedRho25v2 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned2")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v2 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v2"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned2")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v2 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v2"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v2"))
)

MYbtagSequenceNIremovedRho25v2 = cms.Sequence(
	nuclearInteractionsRemoved2 *
	pfImpactParameterTagInfosCleanedRho25v2 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v2 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v2 *
	softPFElectronsTagInfos
)

# sequence with NIs removed (version 3, only position + NI id with relaxed IVF cuts)

pfImpactParameterTagInfosCleanedRho25v3 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned3")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v3 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v3"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned3")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v3 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v3"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v3"))
)

MYbtagSequenceNIremovedRho25v3 = cms.Sequence(
	nuclearInteractionsRemoved3 *
	pfImpactParameterTagInfosCleanedRho25v3 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v3 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v3 *
	softPFElectronsTagInfos
)

# sequence with NIs removed (version 4, position + nctau cut, NI id with relaxed IVF cuts)

pfImpactParameterTagInfosCleanedRho25v4 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned4")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v4 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v4"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned4")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v4 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v4"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v4"))
)

MYbtagSequenceNIremovedRho25v4 = cms.Sequence(
	nuclearInteractionsRemoved4 *
	pfImpactParameterTagInfosCleanedRho25v4 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v4 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v4 *
	softPFElectronsTagInfos
)

# sequence with NIs removed (version 5, position + mass + ntracks + nctau cut, NI id with relaxed IVF cuts)

pfImpactParameterTagInfosCleanedRho25v5 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned5")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v5 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v5"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned5")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v5 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v5"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v5"))
)

MYbtagSequenceNIremovedRho25v5 = cms.Sequence(
	nuclearInteractionsRemoved5 *
	pfImpactParameterTagInfosCleanedRho25v5 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v5 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v5 *
	softPFElectronsTagInfos
)

# from here on, rho cut is applied again
# sequence with NIs removed, but with rho=2.5 cut (version 6, simply positions)

pfImpactParameterTagInfosCleanedRho25v6 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned6")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v6 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v6"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned6")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v6 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v6"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v6"))
)

MYbtagSequenceNIremovedRho25v6 = cms.Sequence(
	nuclearInteractionsRemoved6 *
	pfImpactParameterTagInfosCleanedRho25v6 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v6 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v6 *
	softPFElectronsTagInfos
)

# sequence with NIs removed, but with rho=2.5 cut (version 7, position + mass + ntracks cut + NI id with relaxed IVF cuts)

pfImpactParameterTagInfosCleanedRho25v7 = pfImpactParameterTagInfos.clone(
	candidates = cms.InputTag("vertexAndTracksCleaned7")
)

pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v7 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v7"),
	extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned7")
)

pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v7 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v7"),
	                         cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v7"))
)

MYbtagSequenceNIremovedRho25v7 = cms.Sequence(
	nuclearInteractionsRemoved7 *
	pfImpactParameterTagInfosCleanedRho25v7 *
	pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v7 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v7 *
	softPFElectronsTagInfos
)


niRejSeqRhoCut25 = cms.Sequence(
	MYbtagSequenceStandardRho25 *
	MYbtagSequenceNIremovedRho25v0 *
	MYbtagSequenceNIremovedRho25v1 *
	MYbtagSequenceNIremovedRho25v2 *
	MYbtagSequenceNIremovedRho25v3 *
	MYbtagSequenceNIremovedRho25v4 *
	MYbtagSequenceNIremovedRho25v5 *
	MYbtagSequenceNIremovedRho25v6 *
	MYbtagSequenceNIremovedRho25v7
)