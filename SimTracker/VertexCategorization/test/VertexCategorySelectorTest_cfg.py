import FWCore.ParameterSet.Config as cms

process = cms.Process("VertexCategorySelectorTest")

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
#process.load('Configuration/StandardSequences/MixingNoPileUp_cff')
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration/StandardSequences/Generator_cff')
process.load("Configuration.StandardSequences.Reconstruction_cff")
#process.load('Configuration/StandardSequences/VtxSmearedEarly10TeVCollision_cff')
process.load('Configuration/StandardSequences/Sim_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')

#process.load("SimTracker.TrackHistory.Playback_cff")
process.load("SimTracker.TrackHistory.SecondaryVertexTagInfoProxy_cff")
#process.load("SimTracker.TrackHistory.VertexClassifier_cff")
process.load("SimTracker.VertexCategorization.VertexCategorizer_cff")

process.load("RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff")

#from SimTracker.TrackHistory.CategorySelectors_cff import * 

#process.vertexSelector = VertexCategorySelector( 
    #src = cms.InputTag('svTagInfoProxy'),
    #cut = cms.string("is('BWeakDecay') && !is('CWeakDecay')")
#)

process.add_( 
  cms.Service("TFileService",
      fileName = cms.string("VertexCategoriesAnalyzer_00_test.root")
  )
)

process.vertexHistoryAnalyzer = cms.EDAnalyzer("VertexCategorizerTest",
    process.vertexCategorizer
)

process.vertexHistoryAnalyzer.vertexProducer = cms.untracked.InputTag('inclusiveCandidateSecondaryVertices')

#process.vertexHistoryAnalyzer.vertexProducer = 'vertexSelector'

process.GlobalTag.globaltag = 'START53_V27::All'

process.p = cms.Path(process.inclusiveCandidateVertexing * process.vertexHistoryAnalyzer) #process.svTagInfoProxy * process.vertexSelector *

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(500) )
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring() 
process.source = cms.Source ("PoolSource",fileNames = readFiles) 

readFiles.extend( [
	'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_3_0_pre3/src/SimTracker/VertexCategorization/Inputfiles/store__mc__Fall14DR__QCD_Pt_1400to1800_TuneCUETP8M1_13TeV_pythia8__GEN-SIM-RECODEBUG__Flat20to50bx25_MCRUN2_72_V3-v1__20000__0061EF07-8082-E411-B295-E0CB4E19F976.root' ]);
       

#secFiles.extend( [
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0020/FCF778C5-21B6-DF11-8686-00261894395C.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0020/F075DC4C-1AB6-DF11-B548-003048678FF4.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0020/EEFDD629-26B6-DF11-ABA0-0018F3D096B4.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0020/E85410E7-15B6-DF11-81F5-003048D15DCA.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0020/E676EA20-28B6-DF11-907B-003048678FF4.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0020/E05C93A3-30B6-DF11-B701-00261894385D.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0020/BA4E6136-2CB6-DF11-9725-00304867905A.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0020/A682593C-22B6-DF11-8BB1-001A92971B36.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0020/9627A8AB-33B6-DF11-97A8-001A928116AE.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0020/90A0BDF7-3BB6-DF11-9C3B-0030486790B8.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0020/8047858D-1AB6-DF11-913D-003048679046.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0020/12B8D560-49B6-DF11-AE3B-002618943843.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0019/FC0E62CE-FFB5-DF11-98FD-002618943905.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0019/E6385FC5-0BB6-DF11-9F0D-003048678B0A.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0019/B28E775C-FEB5-DF11-AF63-002618943963.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0019/AE2EBCC1-0DB6-DF11-81FC-003048678B1C.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0019/AC808849-0AB6-DF11-AA31-002618943983.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0019/A4B1683A-10B6-DF11-BD22-001A92971BD8.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0019/50984DCF-FEB5-DF11-B4A8-0030486792AC.root',
       #'/store/relval/CMSSW_3_9_0_pre3/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_38Y_V9-v1/0019/38DC3DCC-03B6-DF11-B9FC-001A928116F2.root' ] );

