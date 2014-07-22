# Playback test file

import FWCore.ParameterSet.Config as cms

process = cms.Process('TrackCategoryAnalyzer')

# import of standard configurations
process.load('Configuration/StandardSequences/Services_cff')
process.load('FWCore/MessageService/MessageLogger_cfi')
#process.load('Configuration/StandardSequences/MixingNoPileUp_cff')
#process.load('Configuration/StandardSequences/GeometryExtended_cff')
process.load('Configuration.Geometry.GeometryIdeal_cff')
process.load('Configuration/StandardSequences/MagneticField_cff')
#process.load('Configuration/StandardSequences/Generator_cff')
#process.load('Configuration/StandardSequences/VtxSmearedEarly10TeVCollision_cff')
process.load('Configuration/StandardSequences/Sim_cff')
process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/EventContent/EventContent_cff')

process.load("SimTracker.TrackHistory.OWNPlayback_cff")
process.load("SimTracker.TrackHistory.TrackClassifier_cff")

process.add_( 
  cms.Service("TFileService",
      fileName = cms.string("/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_2_0_pre1/src/SimTracker/TrackHistory/output/OWNTrackHistoryAnalyzerPlayback/OWNTrackHistoryAnalyzer_02_PLAYBACKFIX.root")
  )
)

process.trackCategoriesAnalyzer = cms.EDAnalyzer("OWNTrackCategoriesAnalyzer",
    process.trackClassifier
)

# Other statements
process.GlobalTag.globaltag = 'POSTLS170_V6::All' #'START70_V7::All'

# Path
process.path = cms.Path(process.trackCategoriesAnalyzer) # process.playback

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(20) )

with open ("/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/GenericTTbar-HC-CMSSW_7_0_4_START70_V7-v1-GEN-SIM-RECO.txt") as inputfile:
    inputfilelist = inputfile.readlines()
    
for i in inputfilelist:
    inputfilelist[inputfilelist.index(i)] = i.replace('\n','')

readFiles = cms.untracked.vstring()
#readFiles.extend( inputfilelist )
secFiles = cms.untracked.vstring() 
process.source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles) # secondaryFileNames = secFiles

inputpath = 'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_2_0_pre1/src/SimGeneral/TrackingAnalysis/input/'

readFiles.extend( [
    'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-RECO_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1.root' ] );
    #'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/PYTHIA6_Tauola_TTbar_TuneZ2star_8TeV_RECODEBUG_CMSSW_7_0_0_pre11_PU50ns_START70_V4-v2.root' ] );
    #'%sTTbar_Tauola_13TeV_fullsim_RAWDEBUG-720-FIX-DIGIONLY-PUFILE-007-step1.root'%inputpath ] );

secFiles.extend( [
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-0.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-1.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-2.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-3.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-4.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-5.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-6.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-7.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-8.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-9.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-10.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-11.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-12.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-13.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-14.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-15.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-16.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-17.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-18.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-19.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-20.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-21.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-22.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-23.root',
       'file:/nfs/dust/cms/user/nowatsd/CMSSW/CMSSW_7_0_5_patch1/src/SimTracker/TrackHistory/input/TTbar_13_GEN-SIM-DIGI-RAW-HLTDEBUG_CMSSW_7_0_0_PU50ns_POSTLS170_V6_AlcaCSA14-v1-24.root'       
       ] );


