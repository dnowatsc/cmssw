/*
 *  TrackCategoriesAnalyzer.C
 *
 *  Created by Victor Eduardo Bazterra on 06/17/08.
 *
 */

#include "TH1F.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "SimTracker/TrackHistory/interface/TrackClassifier.h"

//
// class decleration
//

class TrackCategoriesAnalyzer : public edm::EDAnalyzer
{
public:

    explicit TrackCategoriesAnalyzer(const edm::ParameterSet&);
    ~TrackCategoriesAnalyzer();

private:

    virtual void analyze(const edm::Event&, const edm::EventSetup&);

    // Member data

    edm::InputTag trackProducer_;
    edm::InputTag trackingtruth_;

    std::size_t totalTracks_;

    TrackClassifier classifier_;

    TH1F * trackCategories_;
    TH1F * processTypesClassifier_;       /// FIXME: added code
    TH1F * processTypesTP_;       /// FIXME: added code
    TH1F * processTypesTV_;       /// FIXME: added code
    TH1F * daughterTracksTV_;       /// FIXME: added code
    TH1F * sourceTracksTV_;       /// FIXME: added code

    Int_t numberTrackCategories_;
};


TrackCategoriesAnalyzer::TrackCategoriesAnalyzer(const edm::ParameterSet& config) : classifier_(config)
{
    // Get the track collection
    trackProducer_ = config.getUntrackedParameter<edm::InputTag> ( "trackProducer" );
    trackingtruth_ = config.getUntrackedParameter<edm::InputTag> ( "trackingTruth" );

    // Get the file service
    edm::Service<TFileService> fs;

    // Create a sub directory associated to the analyzer
    TFileDirectory directory = fs->mkdir( "TrackCategoriesAnalyzer" );

    // Number of track categories
    numberTrackCategories_ = TrackCategories::Unknown+1;

    // Define a new histograms
    trackCategories_ = fs->make<TH1F>(
                           "Frequency",
                           "Frequency for the different track categories",
                           numberTrackCategories_,
                           -0.5,
                           numberTrackCategories_ - 0.5
                       );
    
    processTypesClassifier_ = fs->make<TH1F>("ProcessTypesClassifier", "Process types of the classified recoTracks", 250, 0, 250); /// FIXME: added code
    processTypesTP_ = fs->make<TH1F>("ProcessTypesTP", "Process types of TrackingParticles", 25, 0, 25); /// FIXME: added code
    processTypesTV_ = fs->make<TH1F>("ProcessTypesTV", "Process types of TrackingVertexs", 25, 0, 25); /// FIXME: added code
    daughterTracksTV_ = fs->make<TH1F>("DaughterTracksTV", "Number of daughter tracks of TrackingVertexs", 20, 0, 20); /// FIXME: added code
    sourceTracksTV_ = fs->make<TH1F>("SourceTracksTV", "Number of source tracks of TrackingVertexs", 20, 0, 20); /// FIXME: added code

    // Set the proper categories names
    for (Int_t i = 0; i < numberTrackCategories_; ++i)
        trackCategories_->GetXaxis()->SetBinLabel(i+1, TrackCategories::Names[i]);
}


TrackCategoriesAnalyzer::~TrackCategoriesAnalyzer() { }


void TrackCategoriesAnalyzer::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
    // Track collection
    edm::Handle<edm::View<reco::Track> > trackCollection;
    event.getByLabel(trackProducer_, trackCollection);
    
    /** FIXME: added code below */
    
    edm::Handle<TrackingParticleCollection> TPCollection;
    event.getByLabel(trackingtruth_, TPCollection);
    
    edm::Handle<TrackingVertexCollection> TVCollection;
    event.getByLabel(trackingtruth_, TVCollection);
    
    for (std::size_t i = 0; i < TPCollection->size(); ++i)
    {
        unsigned int processType = 0;
        
        if (!TPCollection->at(i).trackPSimHit().empty())
            processType = TPCollection->at(i).pSimHit_begin()->processType();
        processTypesTP_->Fill(processType);
    }
    
    for (std::size_t i = 0; i < TVCollection->size(); ++i)
    {
        unsigned int processType = 0;
        
        daughterTracksTV_->Fill(TVCollection->at(i).daughterTracks().size());
        sourceTracksTV_->Fill(TVCollection->at(i).sourceTracks().size());
        
        if (TVCollection->at(i).daughterTracks().empty())
            continue;
        
        if (!(*TVCollection->at(i).daughterTracks_begin())->trackPSimHit().empty())
            processType = (*TVCollection->at(i).daughterTracks_begin())->pSimHit_begin()->processType();
        processTypesTV_->Fill(processType);
    }
    
    /// FIXME: until here */

    // Set the classifier for a new event
    classifier_.newEvent(event, setup);

    // Loop over the track collection.
    for (std::size_t index = 0; index < trackCollection->size(); index++)
    {
        edm::RefToBase<reco::Track> track(trackCollection, index);

        // Classify the tracks
        classifier_.evaluate(track);
        
        /** FIXME: added code below */
        
        unsigned int processType = 0;
        
        if (!classifier_.history().simParticle()->trackPSimHit().empty())
            processType = classifier_.history().simParticle()->pSimHit_begin()->processType();
        
        processTypesClassifier_->Fill(processType);
        
        /// FIXME: until here */

        // Fill the histogram with the categories
        for (Int_t i = 0; i != numberTrackCategories_; ++i)
            if (
                classifier_.is( (TrackCategories::Category) i )
            )
                trackCategories_->Fill(i);
    }
}


DEFINE_FWK_MODULE(TrackCategoriesAnalyzer);

