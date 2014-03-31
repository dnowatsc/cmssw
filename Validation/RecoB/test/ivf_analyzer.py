import FWCore.ParameterSet.Config as cms

process = cms.Process("validation2")
# load the full reconstraction configuration, to make sure we're getting all needed dependencies
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")

process.load("SimTracker.TrackHistory.TrackClassifier_cff")

process.GlobalTag.globaltag = 'START53_V27::All'

process.maxEvents = cms.untracked.PSet(
	input = cms.untracked.int32(-1)
)

process.bTracksAnalyzerSV0 = cms.EDProducer("BTracksAnalyzer",
	trackConfig = cms.PSet(process.trackClassifier),
	trackInputTag = cms.untracked.InputTag("vertexSplitter","vertex0")
)

process.bTracksAnalyzerSV0.trackConfig.enableSimToReco = cms.untracked.bool(True)
process.bTracksAnalyzerSV1 = process.bTracksAnalyzerSV0.clone(trackInputTag = cms.untracked.InputTag("vertexSplitter","vertex1"))
process.bTracksAnalyzerSV2 = process.bTracksAnalyzerSV0.clone(trackInputTag = cms.untracked.InputTag("vertexSplitter","vertex2"))
process.bTracksAnalyzerSV3 = process.bTracksAnalyzerSV0.clone(trackInputTag = cms.untracked.InputTag("vertexSplitter","vertex3"))
process.bTracksAnalyzerSV4 = process.bTracksAnalyzerSV0.clone(trackInputTag = cms.untracked.InputTag("vertexSplitter","vertex4"))

process.bTracksAnalyzerSV0.trackConfig.trackProducer = cms.untracked.InputTag("vertexSplitter","vertex0")
process.bTracksAnalyzerSV1.trackConfig.trackProducer = cms.untracked.InputTag("vertexSplitter","vertex1")
process.bTracksAnalyzerSV2.trackConfig.trackProducer = cms.untracked.InputTag("vertexSplitter","vertex2")
process.bTracksAnalyzerSV3.trackConfig.trackProducer = cms.untracked.InputTag("vertexSplitter","vertex3")
process.bTracksAnalyzerSV4.trackConfig.trackProducer = cms.untracked.InputTag("vertexSplitter","vertex4")

process.pfake = cms.Path(process.bTracksAnalyzerSV0 * process.bTracksAnalyzerSV1 * process.bTracksAnalyzerSV2 * process.bTracksAnalyzerSV3 * process.bTracksAnalyzerSV4)


process.source = cms.Source("PoolSource",
	fileNames = cms.untracked.vstring('file:/scratch/hh/dust/naf/cms/user/dnowatsc/CMSSW/CMSSW_5_3_14/src/Validation/rootfiles/RecoB/btag004-fakefromttbar_2.root')
)

process.outfake = cms.OutputModule("PoolOutputModule",
	fileName = cms.untracked.string('fakebtag_ttbar_btag070_1.root'),
	SelectEvents = cms.untracked.PSet(
		SelectEvents = cms.vstring("pfake"))
)

process.endpath= cms.EndPath(process.outfake)