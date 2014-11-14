import FWCore.ParameterSet.Config as cms

from RecoBTag.SoftLepton.softLepton_cff import *
from RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff import *
from RecoBTag.SecondaryVertex.pfInclusiveSecondaryVertexFinderTagInfos_cfi import *
from RecoBTag.ImpactParameter.pfImpactParameter_cfi import *

from RecoBTag.SecondaryVertex.candidateCombinedSecondaryVertexES_cfi import *
from RecoBTag.SecondaryVertex.pfCombinedSecondaryVertexBJetTags_cfi import *
from RecoBTag.SecondaryVertex.combinedInclusiveSecondaryVertexBJetTags_cfi import *
from RecoBTag.SecondaryVertex.nuclearInteractionIdentificationNew_cff import *


#======================================================================
# run nuclear interaction identifications with the standard
# pfInclusiveSecondaryVertexFinderTagInfos module, so including
# the cut on the SV position at rho=2.5
#======================================================================


# standard CSVIVFv2 sequence

MYbtagSequenceStandardRho25 = cms.Sequence(
	inclusiveCandidateVertexing *
	pfImpactParameterTagInfos *
	pfInclusiveSecondaryVertexFinderTagInfos *
	pfCombinedInclusiveSecondaryVertexV2BJetTags *
	softPFElectronsTagInfos
)

# standard CSVIVFv2 sequence with relaxed rho cut in pfInclusiveSecondaryVertexFinderTagInfos

pfInclusiveSecondaryVertexFinderTagInfosRho90 = pfInclusiveSecondaryVertexFinderTagInfos.clone()
pfInclusiveSecondaryVertexFinderTagInfosRho90.vertexCuts.distVal2dMax = 9.0

pfCombinedInclusiveSecondaryVertexV2BJetTagsRho90 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	tagInfos = cms.VInputTag(
		cms.InputTag("pfImpactParameterTagInfos"),
		cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosRho90")
	)
)

MYbtagSequenceStandardRho90 = cms.Sequence(
	inclusiveCandidateVertexing *
	pfImpactParameterTagInfos *
	pfInclusiveSecondaryVertexFinderTagInfosRho90 *
	pfCombinedInclusiveSecondaryVertexV2BJetTagsRho90 *
	softPFElectronsTagInfos
)

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


seq = (MYbtagSequenceStandardRho25 *
	    # MYbtagSequenceStandardRho90*
		MYbtagSequenceStandardRho9999)




# NI rejection version 0

for rho in rhoCuts :
	for i in range(0, Nversion2) :
		globals()['pfImpactParameterTagInfosCleaned'+rho+'v%s'%i] = pfImpactParameterTagInfos.clone(candidates = "vertexAndTracksCleaned%s"%i)
		if rho == 'Rho25' : rho2 = ''
		else : rho2 = rho
		globals()['pfInclusiveSecondaryVertexFinderTagInfosCleaned'+rho+'v%s'%i] = globals()['pfInclusiveSecondaryVertexFinderTagInfos'+rho2].clone(	
			trackIPTagInfos = 'pfImpactParameterTagInfosCleaned'+rho+'v%s'%i, extSVCollection = "inclusiveSecondaryVerticesCleaned%s"%i)
		globals()['pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned'+rho+'v%s'%i] = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
			tagInfos = cms.VInputTag(cms.InputTag('pfImpactParameterTagInfosCleaned'+rho+'v%s'%i), cms.InputTag('pfInclusiveSecondaryVertexFinderTagInfosCleaned'+rho+'v%s'%i)))
		globals()['MYbtagSequenceNIremoved'+rho+'v%s'%i] = cms.Sequence(
			globals()['nuclearInteractionsRemoved%s'%i] *
			globals()['pfImpactParameterTagInfosCleaned'+rho+'v%s'%i] *
			globals()['pfInclusiveSecondaryVertexFinderTagInfosCleaned'+rho+'v%s'%i] *
			globals()['pfCombinedInclusiveSecondaryVertexV2BJetTagsCleaned'+rho+'v%s'%i] *
			softPFElectronsTagInfos
		)
		seq = seq * globals()['MYbtagSequenceNIremoved'+rho+'v%s'%i]
	
	
# all b-tagging sequences

bTagSeq = cms.Sequence(seq)

# NI rejection version 1

#pfImpactParameterTagInfosCleanedRho25v1 = pfImpactParameterTagInfos.clone(
	#candidates = cms.InputTag("vertexAndTracksCleaned1")
#)

#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v1 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	#trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v1"),
	#extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned1")
#)

#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v1 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	#tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v1"),
	                         #cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v1"))
#)

#MYbtagSequenceNIremovedRho25v1 = cms.Sequence(
	#nuclearInteractionsRemoved1 *
	#pfImpactParameterTagInfosCleanedRho25v1 *
	#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v1 *
	#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v1 *
	#softPFElectronsTagInfos
#)

## NI rejection version 2

#pfImpactParameterTagInfosCleanedRho25v2 = pfImpactParameterTagInfos.clone(
	#candidates = cms.InputTag("vertexAndTracksCleaned2")
#)

#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v2 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	#trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v2"),
	#extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned2")
#)

#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v2 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	#tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v2"),
	                         #cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v2"))
#)

#MYbtagSequenceNIremovedRho25v2 = cms.Sequence(
	#nuclearInteractionsRemoved2 *
	#pfImpactParameterTagInfosCleanedRho25v2 *
	#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v2 *
	#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v2 *
	#softPFElectronsTagInfos
#)

## NI rejection version 3

#pfImpactParameterTagInfosCleanedRho25v3 = pfImpactParameterTagInfos.clone(
	#candidates = cms.InputTag("vertexAndTracksCleaned3")
#)

#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v3 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	#trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v3"),
	#extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned3")
#)

#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v3 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	#tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v3"),
	                         #cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v3"))
#)

#MYbtagSequenceNIremovedRho25v3 = cms.Sequence(
	#nuclearInteractionsRemoved3 *
	#pfImpactParameterTagInfosCleanedRho25v3 *
	#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v3 *
	#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v3 *
	#softPFElectronsTagInfos
#)

## NI rejection version 4

#pfImpactParameterTagInfosCleanedRho25v4 = pfImpactParameterTagInfos.clone(
	#candidates = cms.InputTag("vertexAndTracksCleaned4")
#)

#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v4 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	#trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v4"),
	#extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned4")
#)

#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v4 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	#tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v4"),
	                         #cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v4"))
#)

#MYbtagSequenceNIremovedRho25v4 = cms.Sequence(
	#nuclearInteractionsRemoved4 *
	#pfImpactParameterTagInfosCleanedRho25v4 *
	#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v4 *
	#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v4 *
	#softPFElectronsTagInfos
#)

## NI rejection version 5

#pfImpactParameterTagInfosCleanedRho25v5 = pfImpactParameterTagInfos.clone(
	#candidates = cms.InputTag("vertexAndTracksCleaned5")
#)

#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v5 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	#trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v5"),
	#extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned5")
#)

#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v5 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	#tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v5"),
	                         #cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v5"))
#)

#MYbtagSequenceNIremovedRho25v5 = cms.Sequence(
	#nuclearInteractionsRemoved5 *
	#pfImpactParameterTagInfosCleanedRho25v5 *
	#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v5 *
	#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v5 *
	#softPFElectronsTagInfos
#)

## NI rejection version 6

#pfImpactParameterTagInfosCleanedRho25v6 = pfImpactParameterTagInfos.clone(
	#candidates = cms.InputTag("vertexAndTracksCleaned6")
#)

#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v6 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	#trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v6"),
	#extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned6")
#)

#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v6 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	#tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v6"),
	                         #cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v6"))
#)

#MYbtagSequenceNIremovedRho25v6 = cms.Sequence(
	#nuclearInteractionsRemoved6 *
	#pfImpactParameterTagInfosCleanedRho25v6 *
	#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v6 *
	#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v6 *
	#softPFElectronsTagInfos
#)

## NI rejection version 7

#pfImpactParameterTagInfosCleanedRho25v7 = pfImpactParameterTagInfos.clone(
	#candidates = cms.InputTag("vertexAndTracksCleaned7")
#)

#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v7 = pfInclusiveSecondaryVertexFinderTagInfos.clone(
	#trackIPTagInfos = cms.InputTag("pfImpactParameterTagInfosCleanedRho25v7"),
	#extSVCollection = cms.InputTag("inclusiveSecondaryVerticesCleaned7")
#)

#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v7 = pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
	#tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfosCleanedRho25v7"),
	                         #cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v7"))
#)

#MYbtagSequenceNIremovedRho25v7 = cms.Sequence(
	#nuclearInteractionsRemoved7 *
	#pfImpactParameterTagInfosCleanedRho25v7 *
	#pfInclusiveSecondaryVertexFinderTagInfosCleanedRho25v7 *
	#pfCombinedInclusiveSecondaryVertexV2BJetTagsCleanedRho25v7 *
	#softPFElectronsTagInfos
#)