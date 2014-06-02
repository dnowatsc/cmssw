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

class TrackFilterFromSecondaryVertices : public edm::EDProducer {
public:
  TrackFilterFromSecondaryVertices(const edm::ParameterSet &params);
  
  virtual void produce(edm::Event &event, const edm::EventSetup &es);
  
private:
  
  edm::InputTag				secondaryVertexCollection;
//   edm::InputTag				trackCollection;
  
  
};

TrackFilterFromSecondaryVertices::TrackFilterFromSecondaryVertices(const edm::ParameterSet &params) :
			secondaryVertexCollection(params.getParameter<edm::InputTag>("secondaryVertices"))
// 			trackCollection(params.getParameter<edm::InputTag>("tracks"))
{
  produces<edm::RefToBaseVector<reco::Track> >();
}





void TrackFilterFromSecondaryVertices::produce(edm::Event &event, const edm::EventSetup &es)
{
  using namespace reco;
  
  edm::Handle<VertexCollection> secondaryVertices;
  event.getByLabel(secondaryVertexCollection, secondaryVertices);
  
//   edm::Handle<TrackCollection> tracks;
//   event.getByLabel(trackCollection, tracks);
  
  std::auto_ptr<edm::RefToBaseVector<reco::Track> > filteredTracks(new edm::RefToBaseVector<reco::Track>);
  
  for (std::vector<reco::Vertex>::const_iterator iVertex = secondaryVertices->begin(); iVertex != secondaryVertices->end(); ++iVertex)
  {
    for (reco::Vertex::trackRef_iterator iDaughter = iVertex->tracks_begin(); iDaughter != iVertex->tracks_end(); ++iDaughter)
    {
//       reco::TrackRef iDaughterRef(iDaughter->id(), iDaughter->key(), iDaughter->productGetter());
      filteredTracks->push_back(*iDaughter);
    }
  }
  
  event.put(filteredTracks);
  
  
}

DEFINE_FWK_MODULE(TrackFilterFromSecondaryVertices);
