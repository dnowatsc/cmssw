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
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourMatching.h"
#include "SimDataFormats/JetMatching/interface/JetFlavour.h"

#include "SimTracker/TrackHistory/interface/VertexClassifierByProxy.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h" 
#include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"



//
// class decleration
//
using namespace reco;
using namespace std;
using namespace edm;
#include "DataFormats/GeometryVector/interface/VectorUtil.h"

class VertexHistoryFilter : public edm::EDFilter
{
	public:
		explicit VertexHistoryFilter(const edm::ParameterSet&);
		~VertexHistoryFilter();

	private:

		virtual void beginJob() ;		
		virtual bool filter(edm::Event&, const edm::EventSetup&);
		virtual void endJob() ;
		
// 		edm::InputTag secondaryVertexTagInfoCollection_;
// 		VertexClassifierByProxy< std::vector<reco::Vertex> > vertexclassifier_;
		VertexClassifier vertexclassifier_;
		edm::InputTag vertices_; // trackingTruth_
// 		std::string category_;
		
		

};


VertexHistoryFilter::VertexHistoryFilter(const edm::ParameterSet& pSet)  :
	vertexclassifier_(pSet.getParameter<edm::ParameterSet>("vertexConfig")),
// 	trackingTruth_(pSet.getParameter<edm::InputTag>("trackingTruth")),
// 	secondaryVertexTagInfoCollection_(pSet.getParameter<edm::InputTag>("secondaryVertexTagInfoCollection")),
	vertices_(pSet.getParameter<edm::InputTag>("vertices"))
// 	category_(pSet.getParameter<std::string>("category"))
{
  produces< std::vector<reco::Vertex> >("HadronicProcess");
  produces< std::vector<reco::Vertex> >("MuNuclProcess");
  produces< std::vector<reco::Vertex> >("UnknownProcess");
  produces< std::vector<reco::Vertex> >("UndefinedProcess");
  produces< std::vector<reco::Vertex> >("BWeakDecay");
  produces< std::vector<reco::Vertex> >("CWeakDecay");
  produces< std::vector<reco::Vertex> >("KsDecay");
}

VertexHistoryFilter::~VertexHistoryFilter()
{
}
 

bool VertexHistoryFilter::filter(edm::Event& event, const edm::EventSetup& setup)
{
  vertexclassifier_.newEvent(event, setup);
  
//   VertexHistory const & ni_tracer = vertexclassifier_.history();
  
//   edm::Handle<reco::GenParticleCollection> genParticles;
//   event.getByLabel("genParticles", genParticles);
  
//   edm::Handle<TrackingVertexCollection>  TVCollection;
//   event.getByLabel(trackingTruth_, TVCollection);
  
  edm::Handle<std::vector<reco::Vertex> > Vertices; 
  event.getByLabel(vertices_, Vertices);
  
//   edm::Handle<std::vector<reco::SecondaryVertexTagInfo> > secondaryVertexTagInfos;
//   event.getByLabel(secondaryVertexTagInfoCollection_, secondaryVertexTagInfos);
  
//   edm::Ref< std::vector<reco::Vertex> > SVTagRef(Vertices, 0);
  
  auto_ptr<VertexCollection> hadronicVertices(new VertexCollection);
  auto_ptr<VertexCollection> munuclVertices(new VertexCollection);
  auto_ptr<VertexCollection> unknownVertices(new VertexCollection);
  auto_ptr<VertexCollection> undefinedVertices(new VertexCollection);
  auto_ptr<VertexCollection> bweakVertices(new VertexCollection);
  auto_ptr<VertexCollection> cweakVertices(new VertexCollection);
  auto_ptr<VertexCollection> kshortVertices(new VertexCollection);
  
  for (std::size_t iVertex = 0; iVertex < Vertices->size(); ++iVertex)
  {
    
    edm::Ref< std::vector<reco::Vertex> > vertRef(Vertices, iVertex);
    
    vertexclassifier_.evaluate(vertRef);
    
    if (vertexclassifier_.is(VertexCategories::HadronicProcess)) 
    {
//       std::cout << "test HadronicProcess" << std::endl;
      hadronicVertices->push_back(*vertRef);
    } 
    if (vertexclassifier_.is(VertexCategories::MuNuclProcess))
    {
//       std::cout << "test UnknownProcess" << std::endl;
      munuclVertices->push_back(*vertRef);
    } 
    if (vertexclassifier_.is(VertexCategories::UnknownProcess))
    {
//       std::cout << "test UnknownProcess" << std::endl;
      unknownVertices->push_back(*vertRef);
    } 
    if (vertexclassifier_.is(VertexCategories::UndefinedProcess))
    {
//       std::cout << "test UndefinedProcess" << std::endl;
      undefinedVertices->push_back(*vertRef);
    } 
    if (vertexclassifier_.is(VertexCategories::BWeakDecay))
    {
//       std::cout << "test BWeakDecay" << std::endl;
      bweakVertices->push_back(*vertRef);
    } 
    if (vertexclassifier_.is(VertexCategories::CWeakDecay))
    {
//       std::cout << "test CWeakDecay" << std::endl;
      cweakVertices->push_back(*vertRef);
    } 
    if (vertexclassifier_.is(VertexCategories::KsDecay))
    {
//       std::cout << "test KsDecay" << std::endl;
      kshortVertices->push_back(*vertRef);
    }
  }
  
  event.put(hadronicVertices, "HadronicProcess");
  event.put(munuclVertices, "MuNuclProcess");
  event.put(unknownVertices, "UnknownProcess");
  event.put(undefinedVertices, "UndefinedProcess");
  event.put(bweakVertices, "BWeakDecay");
  event.put(cweakVertices, "CWeakDecay");
  event.put(kshortVertices, "KsDecay");

  
  return true;

}

// ------------ method called once each job just before starting event loop  ------------
void 
VertexHistoryFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
VertexHistoryFilter::endJob() {

   std::cout << std::endl;
 
} 

DEFINE_FWK_MODULE(VertexHistoryFilter);
