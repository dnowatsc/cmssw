// user include files
//#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/Candidate/interface/LeafCandidate.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

// simulation:
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "RecoBTag/SecondaryVertex/interface/SecondaryVertex.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TTree.h"
#include "HepPDT/ParticleID.hh"

#include "SimTracker/VertexHistory/interface/ivftree.hpp"

#include <map>
// #include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
// #include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"

#include "DataFormats/Common/interface/Ref.h"

#include "SimTracker/TrackHistory/interface/VertexClassifier.h"
#include "SimTracker/TrackHistory/interface/TrackClassifier.h"
#include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimTracker/TrackAssociation/interface/TrackAssociatorByHits.h"
#include "SimTracker/VertexAssociation/interface/VertexAssociatorByTracks.h"
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "SimDataFormats/Track/interface/SimTrack.h"

#include "SimTracker/VertexHistory/interface/VertexClassifierWeight.h"

#include "DataFormats/GeometryVector/interface/VectorUtil.h"

// #include "TrackingTools/IPTools/interface/IPTools.h"



//#include <memory>
//#include <TLorentzVector.h>

using namespace edm;
using namespace reco;
using namespace pat;
using namespace std;


// namespace{
//     
// }



class IvfTreeMaker : public edm::EDAnalyzer {

   public:
      explicit IvfTreeMaker(const edm::ParameterSet&);


   private:

      virtual void beginJob();
      virtual void endJob();
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      
      GlobalVector flightDirection(const reco::Vertex &, const reco::Vertex &);
      TV getTrackingVertex(VertexClassifierWeight &) const;
      LorentzVector getTrackVertP4 (TrackingVertexRef &) const;
      
      edm::InputTag primaryVertices_, secondaryVertices_, tpCollection_;
      
      bool debugMsg_;
      
      VertexClassifierWeight vertexClassifier_;

      // ** for output
      edm::Service<TFileService> fs_;
      TTree *tVertex;
      recoVertex vertex;
};

// ************************************************************************


IvfTreeMaker::IvfTreeMaker(const edm::ParameterSet& iConfig):
    primaryVertices_(iConfig.getParameter<edm::InputTag>("primaryVertices")),
    secondaryVertices_(iConfig.getParameter<edm::InputTag>("secondaryVertices")),
    tpCollection_(iConfig.getParameter<edm::InputTag>("trackingParticles")),
    debugMsg_(iConfig.getParameter<bool>("debugMessage")),
    vertexClassifier_(iConfig.getParameter<edm::ParameterSet>("vertexClassifierWeight"))
  
{
    
}


void IvfTreeMaker::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
//     std::cout << "TEST1" << std::endl;
    edm::Handle<std::vector<reco::Vertex> > secondaryVertices; 
    event.getByLabel(secondaryVertices_, secondaryVertices);
    
    edm::Handle<std::vector<reco::Vertex> > primaryVertices; 
    event.getByLabel(primaryVertices_, primaryVertices);
    const reco::VertexCollection & pvs = *primaryVertices;
    
    //   edm::Handle<edm::View<reco::Track> > recoTracks; 
    //   event.getByLabel(recoTracks_, recoTracks);
    
    edm::Handle<TrackingParticleCollection > TPCollectionHandle; 
    event.getByLabel(tpCollection_, TPCollectionHandle);
    
    edm::ESHandle<TrackAssociatorBase> trackAssociator;
    setup.get<TrackAssociatorRecord>().get("TrackAssociatorByHits", trackAssociator);
    
    edm::RefToBaseVector<reco::Track> recoTracks;
    edm::RefVector<TrackingParticleCollection> TPCollection(TPCollectionHandle.id());
    for (unsigned int j=0; j<TPCollectionHandle->size();j++)
	TPCollection.push_back(edm::Ref<TrackingParticleCollection>(TPCollectionHandle,j));
    
    /// use TransientTrackBuilder here?? what is it used for? => provides more services and functions than the ordinary Track class (magnetic field etc.) but is not needed here
    
    for (std::vector<reco::Vertex>::const_iterator iVertex = secondaryVertices->begin(); iVertex != secondaryVertices->end(); ++iVertex)
    {
	for (reco::Vertex::trackRef_iterator iDaughter = iVertex->tracks_begin(); iDaughter != iVertex->tracks_end(); ++iDaughter)
	{
	    recoTracks.push_back(*iDaughter);
	}
    }
    
//     std::cout << "TEST2" << std::endl;
    
    vertexClassifier_.newEvent(recoTracks, TPCollection, event, setup, debugMsg_);
    int vertexCount = 0;
    
    for (std::vector<reco::Vertex>::const_iterator iRecVertex = secondaryVertices->begin(); iRecVertex != secondaryVertices->end(); ++iRecVertex)
    {
	vertexCount++;
	
	if (debugMsg_)
	    std::cout << "Vertex No. " << vertexCount << ":" << std::endl;
	
// 	std::cout << "TEST3" << std::endl;
	vertexClassifier_.evaluate(*iRecVertex);
// 	std::cout << "TEST4" << std::endl;
	
	vertex.p4daughters = iRecVertex->p4();
	vertex.numberTracks = iRecVertex->nTracks(0.);
	vertex.recoPos = iRecVertex->position();
	vertex.flightdir = flightDirection(pvs[0], *iRecVertex);
	
	reco::SecondaryVertex sv = SecondaryVertex(pvs[0], *iRecVertex, vertex.flightdir, true);
	
	vertex.impact2D = sv.dist2d().value();
	vertex.impactSig2D = sv.dist2d().significance();
	vertex.impact3D = sv.dist3d().value();
	vertex.impactSig3D = sv.dist3d().significance();
	
	vertex.thisProcessFlags.resize(vertexClassifier_.getThisProcessFlags().size());
	vertex.historyProcessFlags.resize(vertexClassifier_.getHistoryProcessFlags().size());
	
	for (unsigned int iFlag = 0; iFlag < vertexClassifier_.getThisProcessFlags().size(); ++iFlag)
	    vertex.thisProcessFlags[iFlag] = vertexClassifier_.getThisProcessFlags()[iFlag];
	
	for (unsigned int iFlag = 0; iFlag < vertexClassifier_.getHistoryProcessFlags().size(); ++iFlag)
	    vertex.historyProcessFlags[iFlag] = vertexClassifier_.getHistoryProcessFlags()[iFlag];
	
	vertex.tvquality = vertexClassifier_.getBestTrackingVertex().first.first;
	
	vertex.ndof = iRecVertex->ndof();
	
	vertex.chi2 = iRecVertex->chi2();
	
	vertex.error = iRecVertex->covariance();
	
	vertex.trackingVertex = getTrackingVertex(vertexClassifier_);
	
	tVertex->Fill();
	
// 	std::cout << "TEST5" << std::endl;
    }
}


void IvfTreeMaker::beginJob() {
//     std::cout << "TEST0" << std::endl;
    tVertex = fs_->make<TTree>("vertex","vertex");
  
#define BR(s) tVertex->Branch(#s, &vertex. s)

    BR(p4daughters); // tEvent->Branch("lumiNo", &svevent.lumiNo);
    BR(recoPos);
    BR(flightdir);
    
    BR(numberTracks);
    
    BR(impact2D);
    BR(impactSig2D);
    BR(impact3D);
    BR(impactSig3D);
    
    BR(thisProcessFlags);
    BR(historyProcessFlags);
    
    BR(tvquality);
    BR(ndof);
    BR(chi2);
    BR(error);
    
    BR(trackingVertex);
  
#undef BR
}

void IvfTreeMaker::endJob(){
//     cout << "IvfTreeMaker: total events process: " << nevents_total << "; survived mll cut: " << nevents_dimu << "; selected: " << nevents_selected << endl;
}

GlobalVector IvfTreeMaker::flightDirection(const reco::Vertex &pv, reco::Vertex const &sv){
    GlobalVector res(sv.position().X() - pv.position().X(),
		     sv.position().Y() - pv.position().Y(),
		     sv.position().Z() - pv.position().Z());
    return res;
}

TV IvfTreeMaker::getTrackingVertex(VertexClassifierWeight & vClass) const
{
    TV res;
    TrackingVertexRef tvRef = vClass.getBestTrackingVertex().second;
    res.numberChargedDaughters = vClass.getBestTrackingVertex().first.second;
    
    res.p4daughters = getTrackVertP4(tvRef);
    
    
    res.tvPos.SetX(tvRef->position().x());
    res.tvPos.SetY(tvRef->position().y());
    res.tvPos.SetZ(tvRef->position().z());
    
    res.pdgIdMother = vClass.getTrackingParticlePdgId(tvRef);
    
    return res;
}

LorentzVector IvfTreeMaker::getTrackVertP4 (TrackingVertexRef & trackVert) const
{
    
    LorentzVector sum;
    ROOT::Math::LorentzVector<ROOT::Math::PxPyPzM4D<double> > vec;
    
    for(TrackingVertex::tp_iterator iter = trackVert->daughterTracks_begin();	iter != trackVert->daughterTracks_end(); ++iter)
    {
	vec.SetPx((*iter)->px());
	vec.SetPy((*iter)->py());
	vec.SetPz((*iter)->pz());
	vec.SetM((*iter)->mass());
	sum += vec;
    }
    return sum;
}


DEFINE_FWK_MODULE(IvfTreeMaker);
