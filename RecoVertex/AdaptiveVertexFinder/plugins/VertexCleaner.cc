#include <memory>
#include <set>

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "RecoVertex/AdaptiveVertexFit/interface/AdaptiveVertexFitter.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexUpdator.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexTrackCompatibilityEstimator.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexSmoother.h"

#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"


//==============================================================================================================================================================
//==============================================================================================================================================================
// CHECKS WHETHER A SET OF RECONSTRUCTED SECONDARY VERTICES SHARE ANY TRACKS HAVING A WEIGHT ABOVE 0.5 (WEIGHT WITH WHICH TRACK HAS CONTRIBUTED TO VERTEX FIT)
// WITH A SET OF PRIMARY VERTICES (ONLY IF THEY HAVE A WEIGHT IN THE PRIMARY VERTEX OF ABOVE 0.5 AS WELL) AND KEEPS THOSE THAT DON'T
// >>> IN THIS CASE THE PRIMARY VERTICES ARE VERTICES THAT ARE IDENTIFIED AS ORIGINATING FROM NUCLEAR INTERACTIONS, THE SECONDARY VERTICES ARE ALL RECONSTRUCTED
// SECONDARY VERTICES SO IT EFFECTIVELY REMOVES ALL VERTICES THAT ARE ASSOCIATED WITH NUCLEAR INTERACTIONS
//==============================================================================================================================================================
//==============================================================================================================================================================



class VertexCleaner : public edm::EDProducer {
    public:
	VertexCleaner(const edm::ParameterSet &params);

	virtual void produce(edm::Event &event, const edm::EventSetup &es);

    private:
	bool trackFilter(const reco::TrackRef &track) const;

	edm::InputTag				primaryVertexCollection;
	edm::InputTag				secondaryVertexCollection;
	double					maxFraction;
};

VertexCleaner::VertexCleaner(const edm::ParameterSet &params) :
	primaryVertexCollection(params.getParameter<edm::InputTag>("primaryVertices")),
	secondaryVertexCollection(params.getParameter<edm::InputTag>("secondaryVertices")),
	maxFraction(params.getParameter<double>("maxFraction"))
{
	produces<reco::VertexCollection>();
}

void VertexCleaner::produce(edm::Event &event, const edm::EventSetup &es)
{
       using namespace reco;
       double sigmacut = 3.0;
       double Tini     = 256.;
       double ratio    = 0.25;

       AdaptiveVertexFitter theAdaptiveFitter(
                                            GeometricAnnealing(sigmacut, Tini, ratio),
                                            DefaultLinearizationPointFinder(),
                                            KalmanVertexUpdator<5>(),
                                            KalmanVertexTrackCompatibilityEstimator<5>(),
                                            KalmanVertexSmoother() );


        edm::ESHandle<TransientTrackBuilder> trackBuilder;
        es.get<TransientTrackRecord>().get("TransientTrackBuilder",
                                           trackBuilder);

	edm::Handle<VertexCollection> primaryVertices;
	event.getByLabel(primaryVertexCollection, primaryVertices);
        std::set<reco::TrackRef> pvTracks;
    
        for(std::vector<reco::Vertex>::const_iterator pv = primaryVertices->begin();pv != primaryVertices->end(); ++pv)
        {
         for(std::vector<reco::TrackBaseRef>::const_iterator iter = pv->tracks_begin();
            iter != pv->tracks_end(); iter++) {
                if (pv->trackWeight(*iter) >= 0.5)
                        pvTracks.insert(iter->castTo<reco::TrackRef>());
         }
	}

	edm::Handle<VertexCollection> secondaryVertices;
	event.getByLabel(secondaryVertexCollection, secondaryVertices);

	std::auto_ptr<VertexCollection> recoVertices(new VertexCollection);
	for(std::vector<reco::Vertex>::const_iterator sv = secondaryVertices->begin();
	    sv != secondaryVertices->end(); ++sv) {
	        std::vector<reco::TransientTrack>  selTracks;
	        GlobalPoint ssv(sv->position().x(),sv->position().y(),sv->position().z());
	        for(std::vector<reco::TrackBaseRef>::const_iterator iter = sv->tracks_begin(); iter != sv->tracks_end(); iter++) {
		              if (sv->trackWeight(*iter) >= 0.5) {
			
	               	      if(pvTracks.count(iter->castTo<reco::TrackRef>()) < 0.5 ) {
		                TransientTrack tt = trackBuilder->build(iter->castTo<reco::TrackRef>());
				selTracks.push_back(tt);                           // pushback
                        }
                      }
  	           }	


		TransientVertex singleFitVertex;
                 singleFitVertex = theAdaptiveFitter.vertex(selTracks,ssv);
                if(singleFitVertex.isValid())  { recoVertices->push_back(reco::Vertex(singleFitVertex)); }
	}

	event.put(recoVertices);
}

DEFINE_FWK_MODULE(VertexCleaner);
