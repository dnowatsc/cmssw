#include <memory>
#include <set>


#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/IPTools/interface/IPTools.h"


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
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"


#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"


#include "DataFormats/GeometryVector/interface/VectorUtil.h"


//#define VTXDEBUG

class NuclearInteractionIdentifierDetector : public edm::EDProducer {
    public:
	NuclearInteractionIdentifierDetector(const edm::ParameterSet &params);


	virtual void produce(edm::Event &event, const edm::EventSetup &es);

    private:
	bool trackFilter(const reco::TrackRef &track) const;

	edm::InputTag				primaryVertexCollection;
	edm::InputTag				secondaryVertexCollection;
        edm::InputTag                           beamSpotCollection;
};

NuclearInteractionIdentifierDetector::NuclearInteractionIdentifierDetector(const edm::ParameterSet &params) :
	primaryVertexCollection      (params.getParameter<edm::InputTag>("primaryVertices")),
	secondaryVertexCollection    (params.getParameter<edm::InputTag>("secondaryVertices")),
        beamSpotCollection           (params.getParameter<edm::InputTag>("beamSpot"))
{
	produces<reco::VertexCollection>();
}


void NuclearInteractionIdentifierDetector::produce(edm::Event &event, const edm::EventSetup &es)
{
	using namespace reco;

	edm::Handle<VertexCollection> secondaryVertices;
	event.getByLabel(secondaryVertexCollection, secondaryVertices);
        VertexCollection theSecVertexColl = *(secondaryVertices.product());

        edm::Handle<VertexCollection> primaryVertices;
        event.getByLabel(primaryVertexCollection, primaryVertices);

	std::auto_ptr<VertexCollection> recoVertices(new VertexCollection);

        if(primaryVertices->size()!=0){ 
        const reco::Vertex &pv = (*primaryVertices)[0];
    
        edm::Handle<BeamSpot> beamSpot;
        event.getByLabel(beamSpotCollection, beamSpot);
	
	// INSERT SOMEWHERE HERE CUT ON THE DETECTOR POSITION (DISCARD OTHER CUTS???)

        for(unsigned int ivtx=0; ivtx < theSecVertexColl.size(); ivtx++){
	       const reco::Vertex & sv = theSecVertexColl[ivtx];	
	       float mass=sv.p4().M();
	       float pt=sv.p4().Pt();
	       float transverseBoost=pt/mass;
               GlobalPoint ppv(pv.position().x(),pv.position().y(),pv.position().z());
               GlobalPoint ssv(sv.position().x(),sv.position().y(),sv.position().z());
               GlobalVector flightDir = ssv-ppv;
	       float flightDistance2D = flightDir.perp(); // transverse direction i.e. projection onto rho-phi-plane
	       unsigned int ntracks = sv.nTracks();
	       float z=sv.position().z();
	       float nctau=flightDistance2D/transverseBoost/0.05;  //number of c*taus  <== ????????????????? ==> flight length of B-Meson??
	       float nctauK=flightDistance2D/transverseBoost/2.68;  //number of c*taus  <== ????????????????? ==> 2.68 is flight length of kauon K-short in cm -> nctauK is number of K0 flight lengths in proper time of the particle (converted by dividing by transverseBoost which is measure for velocity of particle) so it gives a confidence level for the probability that the particle is NOT a K0 (if nctauK is very big than it's quite unlikely to be a K0 also if it's very low)
               const reco::Vertex & extVertex = sv;
               GlobalVector vtxDir = GlobalVector(extVertex.p4().X(),extVertex.p4().Y(),extVertex.p4().Z());
	       float deltaR = Geom::deltaR(extVertex.position() - pv.position(), vtxDir);
//                std::cout << "Vtx id: " << ivtx <<" z: " << fabs(z) << " fdist: " << flightDistance2D << " nctau: " << nctau << " ntr: " << ntracks << " mass: " << mass << " nctauK: " << nctauK << " DR: " << deltaR << " charge prod: "<< (*sv.tracks_begin())->charge()*((*(sv.tracks_begin()+1))->charge()) << std::endl;
	       if( (flightDistance2D > 2.5 && ntracks >= 2 && mass > 0.6 && pt > 0.5) /// TODO: REMOVE FLIGHTDISTANCE2D < 4 CUT >> maybe nctau > 2 cut as well???
// 		    or (flightDistance2D > 2.5 && nctau > 3 )			
// 		    or (flightDistance2D > 2.0 && nctau > 5 && ntracks > 2 )
// 		   or (fabs(mass-0.497) < 0.0150 && ntracks == 2 && (*sv.tracks_begin())->charge()*((*(sv.tracks_begin()+1))->charge()) <0 && nctauK < 2 && nctauK > 0.01 && deltaR < 0.01) // check if it is K0...
			) {
//  			std::cout << "NI " << std::endl;
		       recoVertices->push_back(theSecVertexColl[ivtx]);
 			}

        }

        }	
	event.put(recoVertices);
	
	
	
}

DEFINE_FWK_MODULE(NuclearInteractionIdentifierDetector);
