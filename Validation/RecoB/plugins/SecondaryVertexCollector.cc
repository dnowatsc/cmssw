#include <iostream>
#include <map>
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include <DataFormats/VertexReco/interface/Vertex.h>
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"

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

using namespace reco;
using namespace std;
using namespace edm;


class SecondaryVertexCollector : public edm::EDFilter
{
  
public:
  explicit SecondaryVertexCollector(const edm::ParameterSet&);
  ~SecondaryVertexCollector();
  
private:
  
  virtual void beginJob();
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  virtual void endJob();
  
  int counter;
  int maxcount;
  edm::InputTag vertices_; //trackingVertices_;
//   edm::InputTag GenParticles_;
  auto_ptr< std::vector<reco::Vertex> > SVcollectionB;
  auto_ptr< TrackingVertexCollection > TVcollection;
//   auto_ptr< std::vector<reco::Vertex> > SVcollectionNonB;
//   int stats[5];
//   int bcounter;
//   int svcountB, svcountNonB;
  
};

SecondaryVertexCollector::SecondaryVertexCollector (const edm::ParameterSet &pSet) :
	maxcount(pSet.getParameter<int>("MaxCount")),
	vertices_(pSet.getParameter<edm::InputTag>("VertexInput"))
// 	trackingVertices_(pSet.getParameter<edm::InputTag>("TVInput"))
// 	GenParticles_(pSet.getParameter<edm::InputTag>("GenParticleInput"))
{
  counter=1;
//   bcounter=0;
//   stats[0] = 0; stats[1] = 0; stats[2] = 0; stats[3] = 0; stats[4] = 0;
  produces< std::vector<reco::Vertex> >(); // "allSecVertB"
  produces< TrackingVertexCollection>();
//   produces< std::vector<reco::Vertex> >("allSecVertNonB");
  SVcollectionB.reset(new std::vector<reco::Vertex>);
  TVcollection.reset(new TrackingVertexCollection);
//   SVcollectionNonB.reset(new std::vector<reco::Vertex>);
}

SecondaryVertexCollector::~SecondaryVertexCollector()
{
}


bool SecondaryVertexCollector::filter (edm::Event& event, const edm::EventSetup& setup)
{
  edm::Handle< std::vector<reco::Vertex> > SVevent;
  event.getByLabel(vertices_, SVevent);
  
  
  
//   edm::Handle< std::vector<reco::GenParticle> > GenParticles;
//   event.getByLabel(GenParticles_, GenParticles);
  //std::vector<reco::Vertex> & sec_vertices_ev = *(SVevent.product());
  
//   if (SVevent->size()==0) stats[0]++;
//   else if (SVevent->size()==1) stats[1]++;
//   else if (SVevent->size()==2) stats[2]++;
//   else if (SVevent->size()==3) stats[3]++;
//   else if (SVevent->size()>=4) stats[4]++;
  
//   bool bdecay=false;
  
//   for (std::vector<reco::GenParticle>::const_iterator igenpart = GenParticles->begin(); igenpart != GenParticles->end(); ++igenpart) {
//     if (abs(igenpart->pdgId())==5 || abs(igenpart->pdgId()==4)) {bdecay=true; bcounter++; break;}
//   }
//   if (bdecay) {
  for (std::vector<reco::Vertex>::const_iterator ivertex = SVevent->begin(); ivertex != SVevent->end(); ++ivertex)
  {
    SVcollectionB->push_back(*ivertex);    
  }
  
    bool tvexists = false;
  
  edm::Handle< TrackingVertexCollection > TVevent;
  if (event.getByLabel(vertices_, TVevent))
  {
      tvexists = true;
      for (TrackingVertexCollection::const_iterator iTrackingVertex = TVevent->begin(); iTrackingVertex != TVevent->end(); ++iTrackingVertex)
      {
	  TVcollection->push_back(*iTrackingVertex);    
      }
  }
//   } else {
//     for (std::vector<reco::Vertex>::const_iterator ivertex = SVevent->begin(); ivertex != SVevent->end(); ++ivertex) {
//       SVcollectionNonB->push_back(*ivertex);
//     }
//   }
  if (counter<maxcount) {
//     std::cout << counter << endl;
    counter++;
  }
  else {
    cout << "Max Count reached " << endl;
//     svcountB = SVcollectionB->size();
//     svcountNonB = SVcollectionNonB->size();
    event.put(SVcollectionB); // "allSecVertB"
    event.put(TVcollection);
//     event.put(SVcollectionNonB, "allSecVertNonB");
    return true;
  }
  return false;
}


void SecondaryVertexCollector::beginJob ()
{
}


void SecondaryVertexCollector::endJob ()
{
//   cout << "=======================================================" << endl;
//   cout << "EVENTS WITH 0 | 1 | 2 | 3 | 4+ SECONDARY VERTICES FOUND:" << endl;
//   cout << stats[0] << " " << stats[1] << " " << stats[2] << " " << stats[3] << " " << stats[4] << endl;
//   cout << "=======================================================" << endl;
//   cout << "EVENTS WITH | WITHOUT B PRODUCED" << endl;
//   cout << bcounter << " " << counter-bcounter << endl;
//   cout << "=======================================================" << endl;
//   cout << "NUMBER OF SV'S FROM EVENTS WITH | WITHOUT B PRODUCED" << endl;
//   cout << svcountB << " " << svcountNonB << endl;
//   cout << "=======================================================" << endl;
}

DEFINE_FWK_MODULE(SecondaryVertexCollector);