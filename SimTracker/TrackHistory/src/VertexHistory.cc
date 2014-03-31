
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "SimTracker/Records/interface/VertexAssociatorRecord.h"
#include "SimTracker/TrackHistory/interface/VertexHistory.h"

VertexHistory::VertexHistory (
    const edm::ParameterSet & config
) : HistoryBase()
{
    // Name of the track collection
    trackProducer_ = config.getUntrackedParameter<edm::InputTag> ( "trackProducer" );

    // Name of the track collection
    vertexProducer_ = config.getUntrackedParameter<edm::InputTag> ( "vertexProducer" );

    // Name of the traking pariticle collection
    trackingTruth_ = config.getUntrackedParameter<edm::InputTag> ( "trackingTruth" );

    // Track association record
    trackAssociator_ = config.getUntrackedParameter<std::string> ( "trackAssociator" );

    // Track association record
    vertexAssociator_ = config.getUntrackedParameter<std::string> ( "vertexAssociator" );

    // Association by max. value
    bestMatchByMaxValue_ = config.getUntrackedParameter<bool> ( "bestMatchByMaxValue" );

    // Enable RecoToSim association
    enableRecoToSim_ = config.getUntrackedParameter<bool> ( "enableRecoToSim" );

    // Enable SimToReco association
    enableSimToReco_ = config.getUntrackedParameter<bool> ( "enableSimToReco" );

    quality_ = 0.;
}


void VertexHistory::newEvent (
    const edm::Event & event, const edm::EventSetup & setup
)
{
    if ( enableRecoToSim_ || enableSimToReco_ )
    {

        // Track collection
        edm::Handle<edm::View<reco::Track> > trackCollection;
        event.getByLabel(trackProducer_, trackCollection);

        // Tracking particle information
        edm::Handle<TrackingParticleCollection>  TPCollection;
        event.getByLabel(trackingTruth_, TPCollection);

        // Get the track associator
        edm::ESHandle<TrackAssociatorBase> trackAssociator;
        setup.get<TrackAssociatorRecord>().get(trackAssociator_, trackAssociator);

        // Vertex collection
        edm::Handle<edm::View<reco::Vertex> > vertexCollection;
        event.getByLabel(vertexProducer_, vertexCollection);

        // Tracking particle information
        edm::Handle<TrackingVertexCollection>  TVCollection;
        event.getByLabel(trackingTruth_, TVCollection);

        // Get the track associator
        edm::ESHandle<VertexAssociatorBase> vertexAssociator;
        setup.get<VertexAssociatorRecord>().get(vertexAssociator_, vertexAssociator);
	
	//! MODIFICATION: create RefToBaseVector from the daughter tracks of the SVs and RefVector from the TrackingParticleCollection as input for the trackAssociator
	//! QUESTION: what's the difference between a TrackRef (edm::Ref<TrackCollection>) and a RefVector (edm::RefVector<TrackCollection>)
	
	edm::RefToBaseVector<reco::Track> recoSVTracks;	
	for (edm::View<reco::Vertex>::const_iterator iVertex = vertexCollection->begin(); iVertex != vertexCollection->end(); ++iVertex)
	{
	  for (reco::Vertex::trackRef_iterator iDaughter = iVertex->tracks_begin(); iDaughter != iVertex->tracks_end(); ++iDaughter)
	  {
	    recoSVTracks.push_back(*iDaughter);
	  }
	}
	
	// create and fill RefVector from TrackingParticleCollection
	edm::RefVector<TrackingParticleCollection> TPCollectionRefVector(TPCollection.id());
	for (unsigned int j=0; j<TPCollection->size();j++)
	  TPCollectionRefVector.push_back(edm::Ref<TrackingParticleCollection>(TPCollection,j));

        if ( enableRecoToSim_ )
        {
            // Get the map between recovertex -> simvertex
            //! MODIFICATION: changed input of trackRecoToSim
            reco::RecoToSimCollection trackRecoToSim = trackAssociator->associateRecoToSim(recoSVTracks, TPCollectionRefVector, &event);
	    
	    /// DEFAULT INPUT
// 	    reco::RecoToSimCollection trackRecoToSim = trackAssociator->associateRecoToSim(recoSVTracks, TPCollectionRefVector, &event);

            // Calculate the map between recovertex -> simvertex
            recoToSim_ = vertexAssociator->associateRecoToSim(vertexCollection, TVCollection, event, trackRecoToSim);
        }

        if ( enableSimToReco_ )
        {
            // Get the map between recovertex <- simvertex
            reco::SimToRecoCollection
            trackSimToReco = trackAssociator->associateSimToReco (trackCollection, TPCollection, &event);

            // Calculate the map between recovertex <- simvertex
            simToReco_ = vertexAssociator->associateSimToReco(vertexCollection, TVCollection, event, trackSimToReco);
        }

    }
}


bool VertexHistory::evaluate (reco::VertexBaseRef tv)
{

    if ( !enableRecoToSim_ ) return false;

    std::pair<TrackingVertexRef, double> result =  match(tv, recoToSim_, bestMatchByMaxValue_);	/// where does 'match(...) function come from???

    TrackingVertexRef tvr( result.first );
    quality_ = result.second;	/// double quality = (double)matchedDaughterCounter/simDaughterCounter; => # of recoDaughters that are matched to trackingParticles over the numer of simDaughters of the trackingVertex; 1 means that all reconstructed daughters of the recoVertex have a corresponding simTrack and thus the vertex is very well reconstructed

    if ( !tvr.isNull() )
    {
        HistoryBase::evaluate(tvr);

        recovertex_ = tv;

        return true;
    }

    return false;
}

