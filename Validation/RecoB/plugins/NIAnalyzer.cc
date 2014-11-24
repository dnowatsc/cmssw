// -*- C++ -*-
//
// Package:    NIAnalyzer
// Class:      NIAnalyzer
// 
/**\class NIAnalyzer NIAnalyzer.cc Validation/RecoB/src/NIAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Dominik Nowatschin,68/102,2978,
//         Created:  Mon Jan 13 14:35:23 CET 2014
// $Id$
//
//


// system include files
#include <memory>
#include <typeinfo>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

// #include "TH2.h"

#include "DataFormats/Math/interface/Point3D.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/GlobalError.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/TrajectorySeed/interface/PropagationDirection.h"
#include "RecoTracker/TransientTrackingRecHit/interface/TkTransientTrackingRecHitBuilder.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHit.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeed.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"

#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "RecoTracker/TrackProducer/interface/TrackProducerAlgorithm.h"
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateClosestToBeamLine.h"
#include "TrackingTools/PatternTools/interface/TSCBLBuilderNoMaterial.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h" 
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h" 
#include "TrackingTools/TrackFitters/interface/TrajectoryFitterRecord.h" 
#include "TrackingTools/Records/interface/TransientRecHitRecord.h" 

#include "TrackingTools/TrackFitters/interface/TrajectoryFitter.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"


#include "TrackingTools/DetLayers/interface/NavigationSchool.h"
#include "RecoTracker/Record/interface/NavigationSchoolRecord.h"
#include "RecoTracker/Record/interface/CkfComponentsRecord.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTracker.h"

#include "RecoVertex/AdaptiveVertexFit/interface/AdaptiveVertexFitter.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexUpdator.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexTrackCompatibilityEstimator.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexSmoother.h"
// #include "RecoVertex/MultiVertexFit/interface/MultiVertexFitter.h"
#include "RecoVertex/ConfigurableVertexReco/interface/ConfigurableVertexReconstructor.h"
#include "TrackingTools/PatternTools/interface/TwoTrackMinimumDistance.h"
#include "RecoVertex/AdaptiveVertexFinder/interface/TracksClusteringFromDisplacedSeed.h"



// #include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"

#include <math.h>

namespace ni_analyzer
{
	using namespace reco;
	typedef std::pair<Trajectory*, std::pair<reco::Track*,PropagationDirection> > AlgoProduct; 
	typedef std::vector< AlgoProduct >  AlgoProductCollection;
	typedef edm::RefToBase<TrajectorySeed> SeedRef;
		
	bool buildTrack (const TrajectoryFitter * theFitter,
						 const Propagator * thePropagator,
						 std::vector<reco::Track*> & algoResults,
						 TransientTrackingRecHit::RecHitContainer& hits,
						 TrajectoryStateOnSurface& theTSOS,
						 const TrajectorySeed& seed,
						 float ndof,
						 const reco::BeamSpot& bs,
						 SeedRef seedRef,
						 int qualityMask,signed char nLoops,
						 reco::TrackBase::TrackAlgorithm algo_,
						 bool geometricInnerState_);
	
	TrajectoryStateOnSurface getInitialState(const reco::Track * theT,
					   TransientTrackingRecHit::RecHitContainer& hits,
					   const TrackingGeometry * theG,
					   const MagneticField * theMF);
	
	reco::VertexCompositePtrCandidate * refitVertex(reco::VertexCompositePtrCandidate const & vertex, edm::Handle<BeamSpot> beamSpot, std::vector<reco::Track const*> & tracks, edm::ESHandle<TransientTrackBuilder> trackBuilder);
}


//
// class declaration
//

class NIAnalyzer : public edm::EDAnalyzer {
   public:

      explicit NIAnalyzer(const edm::ParameterSet&);
      ~NIAnalyzer();

// //       static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
//       virtual void beginJob() ;
//       virtual void endJob() ;

//       virtual void beginRun(edm::Run const&, edm::EventSetup const&);
//       virtual void endRun(edm::Run const&, edm::EventSetup const&);
//       virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
//       virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);



      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      
//       std::vector<std::string> titles_;
	  
	  edm::EDGetTokenT<edm::View<reco::Candidate> > tokenSecondaryVertexCollection_;
	  edm::EDGetTokenT<reco::VertexCollection > token_primaryVertex_;
	  edm::EDGetTokenT<reco::BeamSpot> bsSrc_;
	  
// 	  std::vector<TH1D*> SVHistosRho_; // *SVhistoParRho, *TVhistoRho


      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
NIAnalyzer::NIAnalyzer(const edm::ParameterSet& pSet) 
// 	titles_(pSet.getParameter<std::vector<std::string> >("titles"))
{
   tokenSecondaryVertexCollection_ = consumes<edm::View<reco::Candidate> >(pSet.getParameter<edm::InputTag>("secondaryVertices"));
   token_primaryVertex_ = consumes<reco::VertexCollection> (pSet.getParameter<edm::InputTag>("primaryVertices"));
   bsSrc_ = consumes<reco::BeamSpot> (pSet.getParameter<edm::InputTag>("beamSpot"));
//    edm::Service<TFileService> fs;
//    for (unsigned int i = 0; i < secondaryVertexTags_.size(); ++i){
// 	   TH1D *SVhistoRho = fs->make<TH1D>(secondaryVertexTags_[i].label().c_str(), "Secondary Vertices Rho Projection", 300, 0, 30);
// 	   TString title = secondaryVertexTags_[i].label()+";Rho;No. of Events";
// 	   SVhistoRho->SetTitle(title);
// 	   SVHistosRho_.push_back(SVhistoRho);
//    }

}


NIAnalyzer::~NIAnalyzer()
{
}


//
// member functions
//

// ------------ method called for each event  ------------
void
NIAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace reco;
	using namespace edm;
	
	edm::Handle<reco::BeamSpot> recoBeamSpotHandle;
	iEvent.getByToken(bsSrc_,recoBeamSpotHandle);
   
	edm::Handle<edm::View<reco::Candidate> > secondaryVertices;
	iEvent.getByToken(tokenSecondaryVertexCollection_, secondaryVertices);
	
	edm::ESHandle<TransientTrackBuilder> builder;
	iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", builder);
	
	edm::Handle<reco::VertexCollection> primaryVertices;
	iEvent.getByToken(token_primaryVertex_,primaryVertices);
	
	edm::ESHandle<TrackerGeometry> trackingGeometry ;
    iSetup.get<TrackerDigiGeometryRecord>().get(trackingGeometry);
	
	const reco::Vertex & primVert = *primaryVertices->begin();
	
	edm::ESHandle<MagneticField> theMF;
	edm::ESHandle<TrajectoryFitter> theFitter;
	edm::ESHandle<Propagator> thePropagator;
// 	edm::ESHandle<MeasurementTracker>  theMeasTk;
	edm::ESHandle<TransientTrackingRecHitBuilder> theBuilder;
// 	getFromES(setup,theG,theMF,theFitter,thePropagator,theMeasTk,theBuilder);
	
// 	if (conf_.exists("useSimpleMF")) useSimpleMF = conf_.getParameter<bool>("useSimpleMF");
	std::string mfName = "";			
// 	if (useSimpleMF){
// 		mfName = conf_.getParameter<std::string>("SimpleMagneticField"); 
// 	}
	iSetup.get<IdealMagneticFieldRecord>().get(mfName, theMF);
	
	std::string fitterName = "KFFittingSmootherWithOutliersRejectionAndRK";			// = conf_.getParameter<std::string>("Fitter");   
	iSetup.get<TrajectoryFitter::Record>().get(fitterName,theFitter);
	
	std::string propagatorName = "RungeKuttaTrackerPropagator";			// = conf_.getParameter<std::string>("Propagator");   
	iSetup.get<TrackingComponentsRecord>().get(propagatorName,thePropagator);
	
// 	edm::ESHandle<NavigationSchool> theSchool;
// 	std::string theNavigationSchool ="SimpleNavigationSchool";
// 	if (conf_.exists("NavigationSchool")) theNavigationSchool= conf_.getParameter<std::string>("NavigationSchool");
// 	else edm::LogWarning("TrackProducerBase")<<" NavigationSchool parameter not set. secondary hit pattern will not be filled.";
// 	if (theNavigationSchool!=""){
// 	iSetup.get<NavigationSchoolRecord>().get(theNavigationSchool, theSchool);

// 	std::string measTkName = "";			// = conf_.getParameter<std::string>("MeasurementTracker");
// 	iSetup.get<CkfComponentsRecord>().get(measTkName,theMeasTk);
	
	std::string builderName = "WithAngleAndTemplate";			// = conf_.getParameter<std::string>("TTRHBuilder");   
	iSetup.get<TransientRecHitRecord>().get(builderName,theBuilder);
// 	}
	
	
	
// 	std::cout << std::endl << ">>>>>> NEW EVENT >>>>>>>>" << std::endl << std::endl;
	
	int count = 0;
	for (edm::View<reco::Candidate>::const_iterator iVertex = secondaryVertices->begin(); iVertex != secondaryVertices->end(); ++iVertex, ++count){
		
		GlobalPoint sv(iVertex->vx(), iVertex->vy(), iVertex->vz());
		GlobalPoint pv(primVert.position().x(), primVert.position().y(), primVert.position().z());
		
		std::cout << "  Vertex " << count << " at rho / phi / z: " << sv.perp() << " / " << sv.phi() << " / " << sv.z() << std::endl;
		
		
		std::vector<reco::Track*> algoResults;
		std::vector<reco::Track const*> newTracks;
		bool reFit = false;
		
		for (size_t iTrack = 0; iTrack < iVertex->numberOfSourceCandidatePtrs(); ++iTrack){
			std::cout << "    Track " << iTrack << ":" << std::endl;
			CandidatePtr trackCand = iVertex->sourceCandidatePtr(iTrack);
			reco::Track const & recoTrack = *trackCand->bestTrack();
			
			
// 			GlobalVector flightDirection = sv-pv;
			
// 			TransientTrack transientTrack = builder->build(trackCand);
			
// 			Measurement1D ip3d = IPTools::signedImpactParameter3D(transientTrack, flightDirection, primVert).second;
// 			Measurement1D ip2d = IPTools::signedTransverseImpactParameter(transientTrack, flightDirection, primVert).second;
			
// 			std::cout << "     3D IP val / error / sig: " << ip3d.value() << " / "  << ip3d.error() << " / " << ip3d.significance() << " / " << std::endl;
			
			std::cout << "     Innermost hit rho / phi / x / y / z: " << recoTrack.innerPosition().rho() << " / " << recoTrack.innerPosition().phi() << " / " << recoTrack.innerPosition().x() << " / " << recoTrack.innerPosition().y() << " / " << recoTrack.innerPosition().z() << std::endl << std::endl;
			
			if (sv.perp()-recoTrack.innerPosition().rho() < 0.2) {
			
// 			recoTrack.hitPattern().print(HitPattern::HitCategory::TRACK_HITS);
			
// 			int hitCount = 0;
			
						
// 			template <class T> void
//			TrackProducerAlgorithm<T>::runWithTrack(const TrackingGeometry * theG,
// 					const MagneticField * theMF,
// 					const TrackCollection& theTCollection,
// 					const TrajectoryFitter * theFitter,
// 					const Propagator * thePropagator,
// 					const TransientTrackingRecHitBuilder* gbuilder,
// 			   		const reco::BeamSpot& bs,
// 					AlgoProductCollection& algoResults)
			
			const TkTransientTrackingRecHitBuilder * builder = dynamic_cast<TkTransientTrackingRecHitBuilder const *>(theBuilder.product());
			assert(builder);
			try{
				float ndof=0;
				PropagationDirection seedDir = recoTrack.seedDirection();

				// WARNING: here we assume that the hits are correcly sorted according to seedDir
				TransientTrackingRecHit::RecHitContainer hits;
				for (trackingRecHit_iterator iHit=recoTrack.recHitsBegin()+1; iHit!=recoTrack.recHitsEnd(); iHit++){
				{
// 				const GeomDet* geomDetId = (*iHit)->det();
// 				PXBDetId pxbDetId((*iHit)->geographicalId());
// // 				DetId detid = (*iHit)->geographicalId();
// 				if((unsigned int)(pxbDetId.subdetId()) == PixelSubdetector::PixelBarrel || (unsigned int)(pxbDetId.subdetId()) == PixelSubdetector::PixelEndcap) {
// 					const SiPixelRecHit * pixelRecHit = dynamic_cast<const SiPixelRecHit *>(*iHit);
// 					if (pixelRecHit) {
// 						if (pixelRecHit->hasPositionAndError()) std::cout << "      >>>>>> HIT HAS POSITION AND ERROR!" << std::endl;
// 						else std::cout << "      <<<<< HIT DOES NOT HAVE POSITION AND ERROR!" << std::endl;
// 						
// 						LocalPoint localPos = pixelRecHit->localPosition();
// // 						std::cout << "TEST" << std::endl;
// // 						std::cout << _pos.perp() << std::endl;
// 						const GeomDet* geomdet = trackingGeometry->idToDet(pixelRecHit->geographicalId()); //!FIXME: the following code always returns null pointer: const GeomDet* geomdet = pixelRecHit->det()
// // 						std::cout << "TEST0" << std::endl;
// // 						Local2DPoint point = Local2DPoint(pixelRecHit->cluster()->x(),pixelRecHit->cluster()->y());
// // 						std::cout << "TEST1" << std::endl;
// 						GlobalPoint _pos;
// 						if (geomdet)
// 							_pos = geomdet->toGlobal(localPos);		//! using localPos = pixelRecHit->localPosition() this gives a reasonable result which is close to recoTrack.innerPosition() but not identical -> deviation ~ O(0.1) cm
// 						else {
// 							std::cout << "     NO GEOMDET INFO AVAILABLE!" << std::endl;
// 							continue;
// 						}
// // 						std::cout << "TEST2" << std::endl;
// // 						GlobalPoint *pixelPos = new GlobalPoint(pixelRecHit->globalPosition());
// 						std::cout << "      Hit " << hitCount << " subDetId | layer | pixel cluster x/y | pixel x/y/z: " << (*iHit)->geographicalId().subdetId() << " | " << pxbDetId.layer() << " | " << pixelRecHit->cluster()->x() << "/" << pixelRecHit->cluster()->y() << " | " << _pos.x() << " / " << _pos.y() << " / " << _pos.z() << std::endl;
// 					} else std::cout << "     NO PIXEL REC HIT!!" << std::endl;
// 				}
				}
				
					if ((**iHit).geographicalId()!=0U)  hits.push_back( (**iHit).cloneForFit(*builder->geometry()->idToDet( (**iHit).geographicalId() ) ) );
				}
	
				TrajectoryStateOnSurface theInitialStateForRefitting = ni_analyzer::getInitialState(&recoTrack,hits,trackingGeometry.product(),theMF.product());

				// the seed has dummy state and hits.What matters for the fitting is the seedDirection;
				if (seedDir==anyDirection){//if anyDirection the seed direction is not stored in the root file: keep same order
					throw cms::Exception("TrackProducer") << "ERROR: trying to refit a track which doesn't have a properly filled 'seed direction' data member" << std::endl;
				}

				const TrajectorySeed seed = TrajectorySeed(PTrajectoryStateOnDet(), TrajectorySeed::recHitContainer(), seedDir);
				reco::TrackBase::TrackAlgorithm algo_=recoTrack.algo();

				//=====  the hits are in the same order as they were in the track::extra.
				FitterCloner fc(theFitter.product(),builder);      
				
				bool geometricInnerState_ = false;
				
				bool ok = ni_analyzer::buildTrack(fc.fitter.get(),thePropagator.product(),algoResults, hits, theInitialStateForRefitting, seed, ndof, *recoBeamSpotHandle, recoTrack.seedRef(),recoTrack.qualityMask(),recoTrack.nLoops(), algo_, geometricInnerState_);
				if(ok) {
					reco::Track const * refittedTrack = algoResults.back();
					newTracks.push_back(refittedTrack);
					reFit = true;
// 					PXBDetId originalPxbDetId((*recoTrack.recHitsBegin())->geographicalId());
// 					PXBDetId refitPxbDetId((*refittedTrack->recHitsBegin())->geographicalId());
// 					std::cout << "TEST0" << std::endl;
					std::cout << "     Track Refit successful!" << std::endl
						<< "       Original track chi2: " << recoTrack.chi2() 
						<< " | refitted track chi2: " << " | " << refittedTrack->chi2() << std::endl
						<< "       Original track pt/ptError: " << recoTrack.pt() << "/" << recoTrack.ptError()
						<< " | refitted track pt/pterror: " << " | " << refittedTrack->pt() << "/" << refittedTrack->ptError() << std::endl
						<< "       Original track phi/phiError: " << recoTrack.phi() << "/" << recoTrack.phiError()
						<< " | refitted track phi/phierror: " << " | " << refittedTrack->phi() << "/" << refittedTrack->phiError() << std::endl
						<< "       Original track eta/etaError: " << recoTrack.eta() << "/" << recoTrack.etaError()
						<< " | refitted track eta/etaerror: " << " | " << refittedTrack->eta() << "/" << refittedTrack->etaError() << std::endl
						<< "       Original track dxy/dxyError: " << recoTrack.dxy(*recoBeamSpotHandle) << "/" << recoTrack.dxyError()
						<< " | refitted track dxy/dxyerror: " << " | " << refittedTrack->dxy(*recoBeamSpotHandle) << "/" << refittedTrack->dxyError() << std::endl
						<< "       Original track dz/dzError: " << recoTrack.dz(recoBeamSpotHandle->position(recoTrack.vz())) << "/" << recoTrack.dzError()
						<< " | refitted track dz/dzerror: " << " | " << refittedTrack->dz(recoBeamSpotHandle->position(refittedTrack->vz())) << "/" << refittedTrack->dzError() << std::endl;
					continue;
// 					std::cout << "     Original track innermost hit position/id/layer | refitted track innermost hit position/id/layer: " << recoTrack.innerPosition().rho() << "/" << originalPxbDetId.subdetId() << "/" << originalPxbDetId.layer() << " | " << refittedTrack->innerPosition().rho() << "/" << refitPxbDetId.subdetId() << "/" << refitPxbDetId.layer() << std::endl;
				}
				else std::cout << "     NOT successful!" << std::endl;
			}catch ( cms::Exception & e){
				std::cout << "Genexception1: " << e.explainSelf() <<"\n";      
				throw;
			}
			
			}
			
			std::cout << "     No tracks refitted, add original Track!" << std::endl;
			newTracks.push_back(trackCand->bestTrack());
  
		}
		
		if (reFit) {
			std::cout << "   Refit Vertex!" << std::endl;
			
			VertexCompositePtrCandidate const * originalVertex = dynamic_cast<VertexCompositePtrCandidate const *>(&*iVertex);
// 			
			VertexCompositePtrCandidate const * newVertex = ni_analyzer::refitVertex(*originalVertex, recoBeamSpotHandle, newTracks, builder);
			
			if (!newVertex) std::cout << "     Vertex fitting NOT successful!" << std::endl;
			else std::cout << "     Yep, successful." << std::endl;
			
		}
		
		
			
	}
	
	// 	     if (reMatchSplitHits_){
// 	   	//re-match hits that belong together
//       		trackingRecHit_iterator next = i; next++;
//                 if (next != theT->recHitsEnd() && (*i)->isValid()){
// 			//check whether hit and nexthit are on glued module
// 			DetId hitId = (**i).geographicalId();
// 
// 			if(hitId.det() == DetId::Tracker) {
// 			  if (hitId.subdetId() == StripSubdetector::TIB || 
// 			      hitId.subdetId() == StripSubdetector::TOB ||
// 			      hitId.subdetId() == StripSubdetector::TEC ||
// 			      hitId.subdetId() == StripSubdetector::TID ){
// 
// 			    SiStripDetId stripId(hitId);
// 			    if (stripId.partnerDetId() == (*next)->geographicalId().rawId()){
// 			      //yes they are parterns in a glued geometry.
// 			      DetId gluedId = stripId.glued();
// 			      const SiStripRecHit2D *mono=0;
// 			      const SiStripRecHit2D *stereo=0;
// 			      if (stripId.stereo()==0){
// 				mono=dynamic_cast<const SiStripRecHit2D *>(&**i);
// 				stereo=dynamic_cast<const SiStripRecHit2D *>(&**next);
// 			      }
// 			      else{
// 				mono=dynamic_cast<const SiStripRecHit2D *>(&**next);
// 				stereo=dynamic_cast<const SiStripRecHit2D *>(&**i);
// 			      }
// 			      if (!mono	|| !stereo){
// 					edm::LogError("TrackProducerAlgorithm")
// 						<<"cannot get a SiStripRecHit2D from the rechit."<<hitId.rawId()
// 						<<" "<<gluedId.rawId()
// 						<<" "<<stripId.partnerDetId()
// 						<<" "<<(*next)->geographicalId().rawId();
// 				}
// 			      LocalPoint pos;//null
// 			      LocalError err;//null
// 			      hits.push_back(std::make_shared<SiStripMatchedRecHit2D>(pos,err, *builder->geometry()->idToDet(gluedId), mono,stereo));
// 			      //the local position and error is dummy but the fitter does not need that anyways
// 			      i++;
// 			      continue;//go to next.
// 			    }//consecutive hits are on parterns module.
// 		  }}//is a strip module
// 	      }//next is not the end of hits
// 	   }//if rematching option is on.
	
//    for (unsigned int iTag = 0; iTag < secondaryVertexTags_.size(); ++iTag){
// 	   Handle< edm::View<reco::Vertex> > recoVert;
// 	   iEvent.getByLabel(secondaryVertexTags_[iTag], recoVert);
// 	   
// 	   //    TString titlesRho = title_+";Rho;No. of Events";
// 	   //    
// 	   //    SVhistoRho->SetTitle(titlesRho);
// 	   
// 	   for (edm::View<reco::Vertex>::const_iterator ivertex = recoVert->begin(); ivertex != recoVert->end(); ++ivertex)
// 	   {
// 		   double secvert_rho = ivertex->position().rho();
// 		   //      double secvert_phi = ivertex->position().phi();
// 		   //      double secvert_z = ivertex->position().z();
// 		   //      double secvert_y = ivertex->position().y();
// 		   //      double secvert_x = ivertex->position().x();
// 		   
// 		   //=======AXIS PARAMETRIZATION==================
// 		   
// 		   //      double secvert_x_par = ivertex->position().x()-(0.086-0.0007*secvert_z);
// 		   //      double secvert_y_par = ivertex->position().y()-(-0.197+0.0007*secvert_z);
// 		   //      double secvert_rho_par = sqrt(secvert_x_par*secvert_x_par+secvert_y_par*secvert_y_par);
// 		   //      double secvert_phi_par = acos(secvert_x_par/secvert_rho_par);
// 		   
// 		   //      SVhistoRhoZ->Fill(secvert_z, secvert_rho);
// 		   //      SVhistoXY->Fill(secvert_x, secvert_y);
// 		   //      SVhistoRhoPhi->Fill(secvert_phi, secvert_rho);
// 		   SVHistosRho_[iTag]->Fill(secvert_rho);
// 		   
// 		   
// 	   }
//    }
   
   
}

bool ni_analyzer::buildTrack (const TrajectoryFitter * theFitter,
						 const Propagator * thePropagator,
						 std::vector<reco::Track*> & algoResults,
						 TransientTrackingRecHit::RecHitContainer& hits,
						 TrajectoryStateOnSurface& theTSOS,
						 const TrajectorySeed& seed,
						 float ndof,
						 const reco::BeamSpot& bs,
						 SeedRef seedRef,
						 int qualityMask,signed char nLoops,
						 reco::TrackBase::TrackAlgorithm algo_,
						 bool geometricInnerState_
 							)						 
{
  //variable declarations

//   PropagationDirection seedDir = seed.direction();
      
  //perform the fit: the result's size is 1 if it succeded, 0 if fails
  Trajectory && trajTmp = theFitter->fitOne(seed, hits, theTSOS,(nLoops>0) ? TrajectoryFitter::looper : TrajectoryFitter::standard);
  if unlikely(!trajTmp.isValid()) {
/*#ifdef VI_DEBUG
    std::cout << "fit failed " << algo_ << ": " <<  hits.size() <<'|' << int(nLoops) << ' ' << std::endl; 
#endif  */   
     return false;
  }
  
  
  auto theTraj = new Trajectory(std::move(trajTmp));
  theTraj->setSeedRef(seedRef);
  
//   statCount.hits(theTraj->foundHits(),theTraj->lostHits());
//   statCount.algo(int(algo_));

  // TrajectoryStateOnSurface innertsos;
  // if (theTraj->direction() == alongMomentum) {
  //  innertsos = theTraj->firstMeasurement().updatedState();
  // } else { 
  //  innertsos = theTraj->lastMeasurement().updatedState();
  // }
  
  ndof = 0;
  for (auto const & tm : theTraj->measurements()) {
    auto const & h = tm.recHitR();
    if (h.isValid()) ndof = ndof + float(h.dimension())*h.weight();  // two virtual calls!
  }
  
  ndof -= 5.f;
  if unlikely(std::abs(theTSOS.magneticField()->nominalValue())<DBL_MIN) ++ndof;  // same as -4
 
  //if geometricInnerState_ is false the state for projection to beam line is the state attached to the first hit: to be used for loopers
  //if geometricInnerState_ is true the state for projection to beam line is the one from the (geometrically) closest measurement to the beam line: to be sued for non-collision tracks
  //the two shouuld give the same result for collision tracks that are NOT loopers
  TrajectoryStateOnSurface stateForProjectionToBeamLineOnSurface;
  if (geometricInnerState_) {
    stateForProjectionToBeamLineOnSurface = theTraj->closestMeasurement(GlobalPoint(bs.x0(),bs.y0(),bs.z0())).updatedState();
  } else {
    if (theTraj->direction() == alongMomentum) {
      stateForProjectionToBeamLineOnSurface = theTraj->firstMeasurement().updatedState();
    } else { 
      stateForProjectionToBeamLineOnSurface = theTraj->lastMeasurement().updatedState();
    }
  }

  if unlikely(!stateForProjectionToBeamLineOnSurface.isValid()){
//     edm::LogError("CannotPropagateToBeamLine")<<"the state on the closest measurement isnot valid. skipping track.";
    delete theTraj;
    return false;
  }
  const FreeTrajectoryState & stateForProjectionToBeamLine=*stateForProjectionToBeamLineOnSurface.freeState();
  
//   LogDebug("TrackProducer") << "stateForProjectionToBeamLine=" << stateForProjectionToBeamLine;
  
  TSCBLBuilderNoMaterial tscblBuilder;
  TrajectoryStateClosestToBeamLine tscbl = tscblBuilder(stateForProjectionToBeamLine,bs);
  
  if unlikely(!tscbl.isValid()) {
    delete theTraj;
    return false;
  }
  
  GlobalPoint v = tscbl.trackStateAtPCA().position();
  math::XYZPoint  pos( v.x(), v.y(), v.z() );
  GlobalVector p = tscbl.trackStateAtPCA().momentum();
  math::XYZVector mom( p.x(), p.y(), p.z() );
  
//   LogDebug("TrackProducer") << "pos=" << v << " mom=" << p << " pt=" << p.perp() << " mag=" << p.mag();
  
  std::auto_ptr<reco::Track> theTrack(new reco::Track(theTraj->chiSquared(),
			     int(ndof),//FIXME fix weight() in TrackingRecHit
			     pos, mom, tscbl.trackStateAtPCA().charge(), 
			     tscbl.trackStateAtPCA().curvilinearError(),
			     algo_));
//   auto theTrack = new reco::Track(theTraj->chiSquared(),
// 			     int(ndof),//FIXME fix weight() in TrackingRecHit
// 			     pos, mom, tscbl.trackStateAtPCA().charge(), 
// 			     tscbl.trackStateAtPCA().curvilinearError(),
// 			     algo_);
  
  theTrack->setQualityMask(qualityMask);
  theTrack->setNLoops(nLoops);
  
//   LogDebug("TrackProducer") << "theTrack->pt()=" << theTrack->pt();
  
//   LogDebug("TrackProducer") <<"track done\n";
  
//   AlgoProduct aProduct(theTraj,std::make_pair(theTrack,seedDir));
  algoResults.push_back(theTrack.get());
  
//   statCount.track(nLoops);

  return true;
} 


TrajectoryStateOnSurface ni_analyzer::getInitialState(const reco::Track * theT,
					   TransientTrackingRecHit::RecHitContainer& hits,
					   const TrackingGeometry * theG,
					   const MagneticField * theMF){

  TrajectoryStateOnSurface theInitialStateForRefitting;
  //the starting state is the state closest to the first hit along seedDirection.
  //avoiding to use transientTrack, it should be faster;
  TrajectoryStateOnSurface innerStateFromTrack=trajectoryStateTransform::innerStateOnSurface(*theT,*theG,theMF);
  TrajectoryStateOnSurface outerStateFromTrack=trajectoryStateTransform::outerStateOnSurface(*theT,*theG,theMF);
  TrajectoryStateOnSurface initialStateFromTrack = 
    ( (innerStateFromTrack.globalPosition()-hits.front()->det()->position()).mag2() <
      (outerStateFromTrack.globalPosition()-hits.front()->det()->position()).mag2() ) ? 
    innerStateFromTrack: outerStateFromTrack;       
  
  // error is rescaled, but correlation are kept.
  initialStateFromTrack.rescaleError(100);
  return initialStateFromTrack;
  /*
  theInitialStateForRefitting = TrajectoryStateOnSurface(initialStateFromTrack.localParameters(),
							 initialStateFromTrack.localError(), 		      
							 initialStateFromTrack.surface(),
							 theMF);
  return theInitialStateForRefitting;
  */
}

reco::VertexCompositePtrCandidate * ni_analyzer::refitVertex(reco::VertexCompositePtrCandidate const & vertex, edm::Handle<BeamSpot> beamSpot, std::vector<reco::Track const*> & tracks, edm::ESHandle<TransientTrackBuilder> trackBuilder)
{
	GlobalPoint originalVertex(vertex.vx(), vertex.vy(), vertex.vz());
	double sigmacut = 3.0;
	double Tini     = 256.;
	double ratio    = 0.25;
// 	VertexDistance3D vdist;
// 	VertexDistanceXY vdist2d;
// 	MultiVertexFitter theMultiVertexFitter;
	AdaptiveVertexFitter theAdaptiveFitter(
                                            GeometricAnnealing(sigmacut, Tini, ratio),
                                            DefaultLinearizationPointFinder(),
                                            KalmanVertexUpdator<5>(),
                                            KalmanVertexTrackCompatibilityEstimator<5>(),
                                            KalmanVertexSmoother() );


// 	std::auto_ptr<VertexCollection> recoVertices(new VertexCollection);
// 	if(!primaryVertices->size()) return
     
// 	const reco::Vertex &pv = primaryVertices->at(0);
        
	std::vector<TransientTrack> tts;
        //Fill transient track vector 
	for(size_t iTrack = 0; iTrack != tracks.size(); ++iTrack) {
// 		TrackRef ref(tracks, track - tracks->begin());
// 		if (!trackFilter(ref))
// 			continue;
//                 if( std::abs(ref->dz(pv.position())) > maxLIP)
// 			continue;
		TransientTrack tt = trackBuilder->build(tracks[iTrack]);
		tt.setBeamSpot(*beamSpot);
		tts.push_back(tt);
	}
	
	TransientVertex singleFitVertex;
	singleFitVertex = theAdaptiveFitter.vertex(tts,originalVertex);
	
	std::auto_ptr<reco::VertexCompositePtrCandidate> refittedVertex;
	
	if(singleFitVertex.isValid()){
		refittedVertex.reset(new reco::VertexCompositePtrCandidate(singleFitVertex));
		std::cout << "     Vertex Fitting successful!" << std::endl;
	}
	
	return refittedVertex.get();
        
}


// // ------------ method called once each job just before starting event loop  ------------
// void 
// NIAnalyzer::beginJob()
// {
// }
// 
// // ------------ method called once each job just after ending the event loop  ------------
// void 
// NIAnalyzer::endJob() 
// {
//   
// }

//define this as a plug-in
DEFINE_FWK_MODULE(NIAnalyzer);
