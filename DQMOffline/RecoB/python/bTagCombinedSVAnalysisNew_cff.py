import FWCore.ParameterSet.Config as cms
from DQMOffline.RecoB.bTagCombinedSVVariablesNew_cff import *


# combinedSecondaryVertex jet tag computer configuration
bTagCombinedSVAnalysisBlockNew = cms.PSet(
    parameters = cms.PSet(
        categoryVariable = cms.string('vertexCategory'),
        categories = cms.VPSet(cms.PSet(
            combinedSVNoVertexVariablesNew,
            combinedSVPseudoVertexVariablesNew,
            combinedSVRecoVertexVariablesNew
        ), 
            cms.PSet(
                combinedSVNoVertexVariablesNew,
                combinedSVPseudoVertexVariablesNew,
                combinedSVRecoVertexVariablesNew
            ), 
            cms.PSet(
                combinedSVNoVertexVariablesNew,
                combinedSVPseudoVertexVariablesNew
            ), 
            cms.PSet(
                combinedSVNoVertexVariablesNew
            ))
    )
)


