import FWCore.ParameterSet.Config as cms

combinedInclusiveSecondaryVertexBJetTags = cms.EDProducer("JetTagProducer",
	jetTagComputer = cms.string('combinedSecondaryVertexComputer'),
	tagInfos = cms.VInputTag(cms.InputTag("impactParameterTagInfos"),
	                         cms.InputTag("inclusiveSecondaryVertexFinderTagInfos"))
)

pfCombinedInclusiveSecondaryVertexV2BJetTags = cms.EDProducer("JetTagProducer",
        jetTagComputer = cms.string('candidateCombinedSecondaryVertexV2'),
        tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfos"),
                                 cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfos"))
)

