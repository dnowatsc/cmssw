#include <map>
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "SimTracker/TrackHistory/interface/VertexClassifier.h"
#include "SimTracker/TrackHistory/interface/TrackClassifier.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h" 
#include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimTracker/TrackAssociation/interface/TrackAssociatorByHits.h"
#include "SimTracker/VertexAssociation/interface/VertexAssociatorByTracks.h"
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "SimDataFormats/Track/interface/SimTrack.h"

#include "SimTracker/VertexHistory/interface/VertexClassifierWeight.h"

#include "DataFormats/GeometryVector/interface/VectorUtil.h"

#define ARRAY_SIZE 34



//
// class decleration
//
using namespace reco;
using namespace std;
using namespace edm;


class VertexHistoryFilterOwn : public edm::EDFilter
{
	public:
		explicit VertexHistoryFilterOwn(const edm::ParameterSet&);
		~VertexHistoryFilterOwn();
		
		struct Collections
		{
		    auto_ptr<TrackCollection> tracks_[ARRAY_SIZE];
		    auto_ptr<VertexCollection> vertices_[ARRAY_SIZE];
		    auto_ptr<TrackingParticleCollection> trackingParticles_[ARRAY_SIZE];
		    auto_ptr<TrackingVertexCollection> trackingVertices_[ARRAY_SIZE];
		    auto_ptr<std::vector<SimTrack> > simTracks_[ARRAY_SIZE];
// 		    auto_ptr<std::vector<HepMC::GenParticle> > genParticles_[ARRAY_SIZE];
		};
		
		enum Category
		  {
		      Any = 0,
		      Hadronic,
		      Unknown,
		      Undefined,
		      GeantPrimary,
		      Primary,
		      Decay,
		      Compton,
		      Annihilation,
		      EIoni,
		      HIoni,
		      MuIoni,
		      Photon,
		      MuPairProd,
		      Conversions,
		      EBrem,
		      SynchrotronRadiation,
		      MuBrem,
		      MuNucl,
		      BWeak,
		      CWeak,
		      Ks,
		      Proton,
		      ChargePion,
		      ChargeKaon,
		      Tau,
		      Lambda,
		      Jpsi,
		      Xi,
		      SigmaPlus,
		      SigmaMinus,
		      BHist,
		      CHist,
		      KsHist
		  };
		  
		void fillOutputCollections (Collections &, reco::Vertex const &, Category, VertexClassifierWeight &);

	private:
	    
		std::vector<std::string> categories;

		virtual void beginJob() ;		
		virtual bool filter(edm::Event&, const edm::EventSetup&);
		virtual void endJob() ;
		
		edm::InputTag recoVertices_, tpCollection_; // trackingTruth_
// 		std::string category_;
		double discriminator_;
		
		bool debugMsg_;

		VertexClassifierWeight vertexClassifier_;

};


VertexHistoryFilterOwn::VertexHistoryFilterOwn(const edm::ParameterSet& pSet)  :
	recoVertices_(pSet.getParameter<edm::InputTag>("recoVertices")),
	tpCollection_(pSet.getParameter<edm::InputTag>("trackingParticles")),
	discriminator_(pSet.getParameter<double>("discriminator")),
	debugMsg_(pSet.getParameter<bool>("debugMessage")),
	vertexClassifier_(pSet.getParameter<edm::ParameterSet>("vertexClassifierWeight"))
{
    const char* catchar[] = {"Any", "Hadronic", "Unknown", "Undefined", "GeantPrimary", "Primary", "Decay", "Compton", "Annihilation", "EIoni", "HIoni", "MuIoni", "Photon", "MuPairProd", "Conversions", "EBrem", "SynchrotronRadiation", "MuBrem", "MuNucl", "BWeak", "CWeak", "Ks", "Proton", "ChargePion", "ChargeKaon", "Tau", "Lambda", "Jpsi", "Xi", "SigmaPlus", "SigmaMinus", "BHist", "CHist", "KsHist"};
    for (unsigned int i = 0; i < ARRAY_SIZE; ++i)
	categories.push_back(catchar[i]);
    
    for (unsigned int i = 0; i < ARRAY_SIZE; ++i)
    {
	stringstream str;
	str << categories[i] << "Process";
	
	produces <std::vector<reco::Vertex> >(str.str().c_str());
	produces <TrackingParticleCollection>(str.str().c_str());
	produces <TrackCollection>(str.str().c_str());
	produces <TrackingVertexCollection>(str.str().c_str());
	produces <std::vector<SimTrack> >(str.str().c_str());
// 	produces <std::vector<HepMC::GenParticle> >(str.str().c_str());
    }
  
}

VertexHistoryFilterOwn::~VertexHistoryFilterOwn()
{
}
 

bool VertexHistoryFilterOwn::filter(edm::Event& event, const edm::EventSetup& setup)
{
  
//   VertexHistory const & ni_tracer = vertexclassifier_.history();
  
//   edm::Handle<reco::GenParticleCollection> genParticles;
//   event.getByLabel("genParticles", genParticles);
  
//   edm::Handle<TrackingVertexCollection>  TVCollection;
//   event.getByLabel(trackingTruth_, TVCollection);
  
  edm::Handle<std::vector<reco::Vertex> > recoVertices; 
  event.getByLabel(recoVertices_, recoVertices);
  
//   edm::Handle<edm::View<reco::Track> > recoTracks; 
//   event.getByLabel(recoTracks_, recoTracks);
  
  edm::Handle<TrackingParticleCollection > TPCollectionHandle; 
  event.getByLabel(tpCollection_, TPCollectionHandle);
  
  edm::ESHandle<TrackAssociatorBase> trackAssociator;
  setup.get<TrackAssociatorRecord>().get("TrackAssociatorByHits", trackAssociator);
  
//   edm::Handle<std::vector<reco::SecondaryVertexTagInfo> > secondaryVertexTagInfos;
//   event.getByLabel(secondaryVertexTagInfoCollection_, secondaryVertexTagInfos);
  
//   edm::Ref< std::vector<reco::Vertex> > SVTagRef(Vertices, 0);
  
  Collections outputCollections;
  
  for (unsigned int i = 0; i < ARRAY_SIZE; ++i)
  {
      outputCollections.vertices_[i].reset(new VertexCollection);
      outputCollections.tracks_[i].reset(new TrackCollection);
      outputCollections.trackingParticles_[i].reset(new TrackingParticleCollection);
      outputCollections.trackingVertices_[i].reset(new TrackingVertexCollection);
      outputCollections.simTracks_[i].reset(new std::vector<SimTrack>);
//       outputCollections.genParticles_[i].reset (new std::vector<HepMC::GenParticle>);
      
  }
  
  edm::RefToBaseVector<reco::Track> recoTracks;
  edm::RefVector<TrackingParticleCollection> TPCollection(TPCollectionHandle.id());
  for (unsigned int j=0; j<TPCollectionHandle->size();j++)
    TPCollection.push_back(edm::Ref<TrackingParticleCollection>(TPCollectionHandle,j));
  
  for (std::vector<reco::Vertex>::const_iterator iVertex = recoVertices->begin(); iVertex != recoVertices->end(); ++iVertex)
  {
    for (reco::Vertex::trackRef_iterator iDaughter = iVertex->tracks_begin(); iDaughter != iVertex->tracks_end(); ++iDaughter)
    {
      recoTracks.push_back(*iDaughter);
    }
  }
  
  std::cout << "==============NEW SELECTOR==============" << std::endl;
  if(debugMsg_)
    std::cout << "RecoVertices Size: " << recoVertices->size() << std::endl << std::endl;
  
  
  vertexClassifier_.newEvent(recoTracks, TPCollection, event, setup, debugMsg_);
  
  int vertexCount = 0;
  
  for (std::vector<reco::Vertex>::const_iterator iRecVertex = recoVertices->begin(); iRecVertex != recoVertices->end(); ++iRecVertex)
  {
    vertexCount++;
    
    if (debugMsg_)
      std::cout << "Vertex No. " << vertexCount << ":" << std::endl;
    vertexClassifier_.evaluate(*iRecVertex);
    if(debugMsg_)
      std::cout << std::endl << std::endl;
    
    //! FIND A NICER WAY TO DO THIS, E.G. USING ENUMS!!!
      
    fillOutputCollections(outputCollections, *iRecVertex, Category::Any, vertexClassifier_);
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::HadronicProcess) > discriminator_) 
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Hadronic, vertexClassifier_);
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::UnknownProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Unknown, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::UndefinedProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Undefined, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::GeantPrimaryProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::GeantPrimary, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::PrimaryProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Primary, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::DecayProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Decay, vertexClassifier_);
      
    }    
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::ComptonProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Compton, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::AnnihilationProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Annihilation, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::EIoniProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::EIoni, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::HIoniProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::HIoni, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::MuIoniProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::MuIoni, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::PhotonProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Photon, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::MuPairProdProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::MuPairProd, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::ConversionsProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Conversions, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::EBremProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::EBrem, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::SynchrotronRadiationProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::SynchrotronRadiation, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::MuBremProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::MuBrem, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::MuNuclProcess) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::MuNucl, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::BWeakDecay) > discriminator_) 
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::BWeak, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::CWeakDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::CWeak, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::KsDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Ks, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::ProtonDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Proton, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::ChargePionDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::ChargePion, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::ChargeKaonDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::ChargeKaon, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::TauDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Tau, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::LambdaDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Lambda, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::JpsiDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Jpsi, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::XiDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Xi, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::SigmaPlusDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::SigmaPlus, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getThisProcessWeight(VertexClassifierWeight::SigmaMinusDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::Ks, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getHistoryWeight(VertexClassifierWeight::BWeakDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::SigmaMinus, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getHistoryWeight(VertexClassifierWeight::CWeakDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::CHist, vertexClassifier_);
      
    }
    
    if (vertexClassifier_.getHistoryWeight(VertexClassifierWeight::KsDecay) > discriminator_)
    {
	fillOutputCollections(outputCollections, *iRecVertex, Category::KsHist, vertexClassifier_);
      
    } 
      
  }
  
  for (unsigned int i=0; i<ARRAY_SIZE; ++i)
  {
      stringstream str;
      str << categories[i] << "Process";
      
      event.put(outputCollections.vertices_[i], str.str().c_str());
      event.put(outputCollections.tracks_[i], str.str().c_str());
      event.put(outputCollections.trackingParticles_[i], str.str().c_str());
      event.put(outputCollections.trackingVertices_[i], str.str().c_str());
      event.put(outputCollections.simTracks_[i], str.str().c_str());
//       event.put(outputCollections.genParticles_[i], str.str().c_str());
  }

  
  return true;

}

void VertexHistoryFilterOwn::fillOutputCollections (Collections & collections, reco::Vertex const & vertex, Category category, VertexClassifierWeight & vertexClassifier)
{
    collections.vertices_[category]->push_back(vertex);
    for (reco::Vertex::trackRef_iterator iDaughter = vertex.tracks_begin(); iDaughter != vertex.tracks_end(); ++iDaughter)
    {
	collections.tracks_[category]->push_back(**iDaughter);
	if ( vertexClassifier.getRecoToSim().numberOfAssociations(*iDaughter) > 0 )
	{
	    std::vector<std::pair<TrackingParticleRef,double> > associations = vertexClassifier.getRecoToSim()[*iDaughter];
	    
	    for (std::vector<std::pair<TrackingParticleRef,double> >::const_iterator association = associations.begin(); association != associations.end(); ++association)
	    {
		collections.trackingParticles_[category]->push_back(*association->first);
		for( TrackingParticle::g4t_iterator g4T=association->first->g4Track_begin(); g4T != association->first->g4Track_end(); ++g4T )
		    collections.simTracks_[category]->push_back(*g4T);
		    
	    }
	}
    }
    
    TrackingVertexRef bestTrackingVertex = vertexClassifier.getBestTrackingVertex(vertexClassifier.getMatchedTrackingVertices()).second;
    
    collections.trackingVertices_[category]->push_back(*bestTrackingVertex);
    if (!bestTrackingVertex->sourceTracks().empty())
    {
	for (TrackingVertex::tp_iterator iSourceTrack = bestTrackingVertex->sourceTracks_begin(); iSourceTrack != bestTrackingVertex->sourceTracks_end(); ++iSourceTrack)
	    collections.trackingParticles_[category]->push_back(**iSourceTrack);
    }
    
//     for (VertexClassifierWeight::MatchedTrackingVertexCollection::const_iterator iTrackVert = vertexClassifier.getMatchedTrackingVertices().begin(); iTrackVert != vertexClassifier.getMatchedTrackingVertices().end(); ++iTrackVert )
//     {
// 	collections.trackingVertices_[category]->push_back(*iTrackVert->first);
// 	if (!iTrackVert->first->sourceTracks().empty())
// 	{
// 	    for (TrackingVertex::tp_iterator iSourceTrack = iTrackVert->first->sourceTracks_begin(); iSourceTrack != iTrackVert->first->sourceTracks_end(); ++iSourceTrack)
// 		collections.trackingParticles_[category]->push_back(**iSourceTrack);
// 	}
// // 	else if ( !iTrackVert->first->genVertices().empty() )
// // 	{
// // 	    for (TrackingVertex::genv_iterator iGenVert = iTrackVert->first->genVertices_begin(); iGenVert != iTrackVert->first->genVertices_end(); ++iGenVert)
// // 		collections.genParticles_[category]->push_back(**(*iGenVert)->particles_in_const_begin());
// // 	    //!===========CONTINUE HERE========
// // 	    //!try to include the genparticles as well
// // 	}
// 	    
//     }
}

// ------------ method called once each job just before starting event loop  ------------
void VertexHistoryFilterOwn::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void VertexHistoryFilterOwn::endJob()
{

   std::cout << std::endl;
 
} 

DEFINE_FWK_MODULE(VertexHistoryFilterOwn);


//===============GARBAGE==================




//     produces< std::vector<reco::Vertex> >("AnyProcess");
//     produces< std::vector<reco::Vertex> >("HadronicProcess");
//     produces< std::vector<reco::Vertex> >("UnknownProcess");
//     produces< std::vector<reco::Vertex> >("UndefinedProcess");
//     produces< std::vector<reco::Vertex> >("DecayProcess");
//     produces< std::vector<reco::Vertex> >("BWeakDecay");
//     produces< std::vector<reco::Vertex> >("CWeakDecay");
//     produces< std::vector<reco::Vertex> >("KsDecay");
//     produces< std::vector<reco::Vertex> >("BWeakInHistory");
//     produces< std::vector<reco::Vertex> >("CWeakInHistory");
//     produces< std::vector<reco::Vertex> >("KsInHistory");
//     produces<TrackingParticleCollection>("AnyProcess");
//     produces<TrackingParticleCollection>("HadronicProcess");
//     produces<TrackingParticleCollection>("UnknownProcess");
//     produces<TrackingParticleCollection>("UndefinedProcess");
//     produces<TrackingParticleCollection>("DecayProcess");
//     produces<TrackingParticleCollection>("BWeakDecay");
//     produces<TrackingParticleCollection>("CWeakDecay");
//     produces<TrackingParticleCollection>("KsDecay");
//     produces<TrackingParticleCollection>("BWeakInHistory");
//     produces<TrackingParticleCollection>("CWeakInHistory");
//     produces<TrackingParticleCollection>("KsInHistory");
//     produces<TrackCollection>("AnyProcess");
//     produces<TrackCollection>("HadronicProcess");
//     produces<TrackCollection>("UnknownProcess");
//     produces<TrackCollection>("UndefinedProcess");
//     produces<TrackCollection>("DecayProcess");
//     produces<TrackCollection>("BWeakDecay");
//     produces<TrackCollection>("CWeakDecay");
//     produces<TrackCollection>("KsDecay");
//     produces<TrackCollection>("BWeakInHistory");
//     produces<TrackCollection>("CWeakInHistory");
//     produces<TrackCollection>("KsInHistory");
//     produces<std::vector<TrackingVertexCollection>>("AnyProcess");
//     produces<std::vector<TrackingVertexCollection>>("HadronicProcess");
//     produces<std::vector<TrackingVertexCollection>>("UnknownProcess");
//     produces<std::vector<TrackingVertexCollection>>("UndefinedProcess");
//     produces<std::vector<TrackingVertexCollection>>("DecayProcess");
//     produces<std::vector<TrackingVertexCollection>>("BWeakDecay");
//     produces<std::vector<TrackingVertexCollection>>("CWeakDecay");
//     produces<std::vector<TrackingVertexCollection>>("KsDecay");
//     produces<std::vector<TrackingVertexCollection>>("BWeakInHistory");
//     produces<std::vector<TrackingVertexCollection>>("CWeakInHistory");
//     produces<std::vector<TrackingVertexCollection>>("KsInHistory");
//     produces<std::vector<SimTrack>>("AnyProcess");
//     produces<std::vector<SimTrack>>("HadronicProcess");
//     produces<std::vector<SimTrack>>("UnknownProcess");
//     produces<std::vector<SimTrack>>("UndefinedProcess");
//     produces<std::vector<SimTrack>>("DecayProcess");
//     produces<std::vector<SimTrack>>("BWeakDecay");
//     produces<std::vector<SimTrack>>("CWeakDecay");
//     produces<std::vector<SimTrack>>("KsDecay");
//     produces<std::vector<SimTrack>>("BWeakInHistory");
//     produces<std::vector<SimTrack>>("CWeakInHistory");
//     produces<std::vector<SimTrack>>("KsInHistory");



//   auto_ptr<VertexCollection> verticesPut[11](new VertexCollection);
//   auto_ptr<TrackingParticleCollection> trackingParticlesPut[11](new TrackingParticleCollection);
//   auto_ptr<TrackCollection> tracksPut[11](new TrackCollection);
//   auto_ptr<TrackingVertexCollection> trackingVerticesPut[11](new TrackingVertexCollection);
//   auto_ptr<std::vector<SimTrack>> simTracksPut[11](new std::vector<SimTrack>);

//   auto_ptr<VertexCollection> anyVertices(new VertexCollection);
//   auto_ptr<VertexCollection> hadronicVertices(new VertexCollection);
//   auto_ptr<VertexCollection> unknownVertices(new VertexCollection);
//   auto_ptr<VertexCollection> undefinedVertices(new VertexCollection);
//   auto_ptr<VertexCollection> decayVertices(new VertexCollection);
//   auto_ptr<VertexCollection> bVertices(new VertexCollection);
//   auto_ptr<VertexCollection> cVertices(new VertexCollection);
//   auto_ptr<VertexCollection> ksVertices(new VertexCollection);
//   auto_ptr<VertexCollection> bHistVertices(new VertexCollection);
//   auto_ptr<VertexCollection> cHistVertices(new VertexCollection);
//   auto_ptr<VertexCollection> ksHistVertices(new VertexCollection);
// 
//   auto_ptr<TrackingParticleCollection> anyTrackingParticles(new TrackingParticleCollection);
//   auto_ptr<TrackingParticleCollection> hadronicTrackingParticles(new TrackingParticleCollection);
//   auto_ptr<TrackingParticleCollection> unknownTrackingParticles(new TrackingParticleCollection);
//   auto_ptr<TrackingParticleCollection> undefinedTrackingParticles(new TrackingParticleCollection);
//   auto_ptr<TrackingParticleCollection> decayTrackingParticles(new TrackingParticleCollection);
//   auto_ptr<TrackingParticleCollection> bTrackingParticles(new TrackingParticleCollection);
//   auto_ptr<TrackingParticleCollection> cTrackingParticles(new TrackingParticleCollection);
//   auto_ptr<TrackingParticleCollection> ksTrackingParticles(new TrackingParticleCollection);
//   auto_ptr<TrackingParticleCollection> bHistTrackingParticles(new TrackingParticleCollection);
//   auto_ptr<TrackingParticleCollection> cHistTrackingParticles(new TrackingParticleCollection);
//   auto_ptr<TrackingParticleCollection> ksHistTrackingParticles(new TrackingParticleCollection);
//   
//   auto_ptr<TrackCollection> anyTracks(new TrackCollection);
//   auto_ptr<TrackCollection> hadronicTracks(new TrackCollection);
//   auto_ptr<TrackCollection> unknownTracks(new TrackCollection);
//   auto_ptr<TrackCollection> undefinedTracks(new TrackCollection);
//   auto_ptr<TrackCollection> decayTracks(new TrackCollection);
//   auto_ptr<TrackCollection> bTracks(new TrackCollection);
//   auto_ptr<TrackCollection> cTracks(new TrackCollection);
//   auto_ptr<TrackCollection> ksTracks(new TrackCollection);
//   auto_ptr<TrackCollection> bHistTracks(new TrackCollection);
//   auto_ptr<TrackCollection> cHistTracks(new TrackCollection);
//   auto_ptr<TrackCollection> ksHistTracks(new TrackCollection);
//   
//   auto_ptr<TrackingVertexCollection> anyTrackingVertices(new TrackingVertexCollection);
//   auto_ptr<TrackingVertexCollection> hadronicTrackingVertices(new TrackingVertexCollection);
//   auto_ptr<TrackingVertexCollection> unknownTrackingVertices(new TrackingVertexCollection);
//   auto_ptr<TrackingVertexCollection> undefinedTrackingVertices(new TrackingVertexCollection);
//   auto_ptr<TrackingVertexCollection> decayTrackingVertices(new TrackingVertexCollection);
//   auto_ptr<TrackingVertexCollection> bTrackingVertices(new TrackingVertexCollection);
//   auto_ptr<TrackingVertexCollection> cTrackingVertices(new TrackingVertexCollection);
//   auto_ptr<TrackingVertexCollection> ksTrackingVertices(new TrackingVertexCollection);
//   auto_ptr<TrackingVertexCollection> bHistTrackingVertices(new TrackingVertexCollection);
//   auto_ptr<TrackingVertexCollection> cHistTrackingVertices(new TrackingVertexCollection);
//   auto_ptr<TrackingVertexCollection> ksHistTrackingVertices(new TrackingVertexCollection);
//   
//   auto_ptr<std::vector<SimTrack>> anySimTracks(new std::vector<SimTrack>);
//   auto_ptr<std::vector<SimTrack>> hadronicSimTracks(new std::vector<SimTrack>);
//   auto_ptr<std::vector<SimTrack>> unknownSimTracks(new std::vector<SimTrack>);
//   auto_ptr<std::vector<SimTrack>> undefinedSimTracks(new std::vector<SimTrack>);
//   auto_ptr<std::vector<SimTrack>> decaySimTracks(new std::vector<SimTrack>);
//   auto_ptr<std::vector<SimTrack>> bSimTracks(new std::vector<SimTrack>);
//   auto_ptr<std::vector<SimTrack>> cSimTracks(new std::vector<SimTrack>);
//   auto_ptr<std::vector<SimTrack>> ksSimTracks(new std::vector<SimTrack>);
//   auto_ptr<std::vector<SimTrack>> bHistSimTracks(new std::vector<SimTrack>);
//   auto_ptr<std::vector<SimTrack>> cHistSimTracks(new std::vector<SimTrack>);
//   auto_ptr<std::vector<SimTrack>> ksHistSimTracks(new std::vector<SimTrack>);






//     event.put(anyVertices, "AnyProcess");
//     event.put(hadronicVertices, "HadronicProcess");
//     event.put(unknownVertices, "UnknownProcess");
//     event.put(undefinedVertices, "UndefinedProcess");
//     event.put(decayVertices, "DecayProcess");
//     event.put(bVertices, "BWeakDecay");
//     event.put(cVertices, "CWeakDecay");
//     event.put(ksVertices, "KsDecay");
//     event.put(bHistVertices, "BWeakInHistory");
//     event.put(cHistVertices, "CWeakInHistory");
//     event.put(ksHistVertices, "KsInHistory");  
//     
//     event.put(anyTrackingParticles, "AnyProcess");
//     event.put(hadronicTrackingParticles, "HadronicProcess");
//     event.put(unknownTrackingParticles, "UnknownProcess");
//     event.put(undefinedTrackingParticles, "UndefinedProcess");
//     event.put(decayTrackingParticles, "DecayProcess");
//     event.put(bTrackingParticles, "BWeakDecay");
//     event.put(cTrackingParticles, "CWeakDecay");
//     event.put(ksTrackingParticles, "KsDecay");
//     event.put(bHistTrackingParticles, "BWeakInHistory");
//     event.put(cHistTrackingParticles, "CWeakInHistory");
//     event.put(ksHistTrackingParticles, "KsInHistory");  
//     
//     event.put(anyTracks, "AnyProcess");
//     event.put(hadronicTracks, "HadronicProcess");
//     event.put(unknownTracks, "UnknownProcess");
//     event.put(undefinedTracks, "UndefinedProcess");
//     event.put(decayTracks, "DecayProcess");
//     event.put(bTracks, "BWeakDecay");
//     event.put(cTracks, "CWeakDecay");
//     event.put(ksTracks, "KsDecay");
//     event.put(bHistTracks, "BWeakInHistory");
//     event.put(cHistTracks, "CWeakInHistory");
//     event.put(ksHistTracks, "KsInHistory");  
//     
//     event.put(anyTrackingVertices, "AnyProcess");
//     event.put(hadronicTrackingVertices, "HadronicProcess");
//     event.put(unknownTrackingVertices, "UnknownProcess");
//     event.put(undefinedTrackingVertices, "UndefinedProcess");
//     event.put(decayTrackingVertices, "DecayProcess");
//     event.put(bTrackingVertices, "BWeakDecay");
//     event.put(cTrackingVertices, "CWeakDecay");
//     event.put(ksTrackingVertices, "KsDecay");
//     event.put(bHistTrackingVertices, "BWeakInHistory");
//     event.put(cHistTrackingVertices, "CWeakInHistory");
//     event.put(ksHistTrackingVertices, "KsInHistory");
//     
//     event.put(anySimTracks, "AnyProcess");
//     event.put(hadronicSimTracks, "HadronicProcess");
//     event.put(unknownSimTracks, "UnknownProcess");
//     event.put(undefinedSimTracks, "UndefinedProcess");
//     event.put(decaySimTracks, "DecayProcess");
//     event.put(bSimTracks, "BWeakDecay");
//     event.put(cSimTracks, "CWeakDecay");
//     event.put(ksSimTracks, "KsDecay");
//     event.put(bHistSimTracks, "BWeakInHistory");
//     event.put(cHistSimTracks, "CWeakInHistory");
//     event.put(ksHistSimTracks, "KsInHistory");






//       std::cout << "test UnknownProcess" << std::endl;
//       ksHistVertices->push_back(*iRecVertex);
//       for (reco::Vertex::trackRef_iterator iDaughter = iRecVertex->tracks_begin(); iDaughter != iRecVertex->tracks_end(); ++iDaughter)
//       {
    // 	  ksHistTracks->push_back(**iDaughter);
// 	  if ( vertexClassifier_.getRecoToSim().numberOfAssociations(*iDaughter) > 0 )
// 	  {
    // 	      std::vector<std::pair<TrackingParticleRef,double> > associations = vertexClassifier_.getRecoToSim()[*iDaughter];
// 	      
// 	      for (std::vector<std::pair<TrackingParticleRef,double> >::const_iterator association = associations.begin(); association != associations.end(); ++association)
// 	      {
    // 		  ksHistTrackingParticles->push_back(*association->first);
// 		  for( TrackingParticle::g4t_iterator g4T=association->first->g4Track_begin(); g4T != association->first->g4Track_end(); ++g4T )
// 		      ksHistSimTracks->push_back(*g4T)
// 		      
// 	      }
// 	  }
//       }
//       
//       for (VertexClassifierWeight::MatchedTrackingVertexCollection::const_iterator iTrackVert = vertexClassifier_.getMatchedTrackingVertices().begin(); iTrackVert != vertexClassifier_.getMatchedTrackingVertices().end(); ++iTrackVert )
// 	  ksHistTrackingVertices->push_back(*iTrackVert->first);
