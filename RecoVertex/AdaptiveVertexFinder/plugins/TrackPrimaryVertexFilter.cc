#include <memory>

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"


//#define VTXDEBUG 1

class TrackPrimaryVertexFilter : public edm::EDProducer {
    public:
	TrackPrimaryVertexFilter(const edm::ParameterSet &params);

	virtual void produce(edm::Event &event, const edm::EventSetup &es);

    private:

	edm::InputTag				primaryVertexCollection;
	edm::InputTag				trackCollection;
	

};

TrackPrimaryVertexFilter::TrackPrimaryVertexFilter(const edm::ParameterSet &params) :
	primaryVertexCollection(params.getParameter<edm::InputTag>("primaryVertices")),
	trackCollection(params.getParameter<edm::InputTag>("tracks"))
{
	produces<reco::TrackCollection>();
}





void TrackPrimaryVertexFilter::produce(edm::Event &event, const edm::EventSetup &es)
{
	using namespace reco;
	
	edm::Handle<VertexCollection> primaryVertices;
	event.getByLabel(primaryVertexCollection, primaryVertices);
	
	edm::Handle<TrackCollection> tracks;
	event.getByLabel(trackCollection, tracks);
	
	std::auto_ptr<TrackCollection> filteredTracks(new TrackCollection);
	
	for (reco::TrackCollection::const_iterator itrack = tracks->begin(); itrack != tracks->end(); ++itrack) {
	  TrackRef ref(tracks, itrack - tracks->begin());
	  bool isFromPrimVert = false;
	  for (reco::VertexCollection::const_iterator iPrimVert = primaryVertices->begin()+1; iPrimVert != primaryVertices->end(); ++iPrimVert) {
	    double distToPrimVert = abs(iPrimVert->position().z()-(*primaryVertices)[0].position().z());
	    if (iPrimVert->ndof() >= 4 && distToPrimVert < 24 && iPrimVert->trackWeight(ref) >= 0.5) isFromPrimVert = true; /// USE DIFFERENT TRACK WEIGHT??? ALSO IMPOSE REQUIREMENT ON ERROR OF VERTEX???    
	  }
	  if (!isFromPrimVert) filteredTracks->push_back(*ref);
	}
	
	event.put(filteredTracks);
  

}

DEFINE_FWK_MODULE(TrackPrimaryVertexFilter);
