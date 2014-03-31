#include <map>
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
//#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
/*#include <string>
 * #include <sstream>
 * #include <cmath>
 * #include <Math/Functions.h>
 * #include <Math/SVector.h>
 * #include <Math/SMatrix.h>
 * #include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
 * #include "DataFormats/SiPixelDetId/interface/PXFDetId.h"
 * #include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
 * #include "TH1F.h"
 * #include "TH1D.h"
 * #include "TH2D.h"
 * #include "TMath.h"
 * #include "TTree.h"
 * #include <DataFormats/PatCandidates/interface/Jet.h>
 * #include <DataFormats/JetReco/interface/PFJet.h>
 * #include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"
 * #include "Validation/RecoVertex/interface/TrackParameterAnalyzer.h"
 */
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

/*#include "DQMServices/Core/interface/DQMStore.h"
 * #include "DQMServices/Core/interface/MonitorElement.h"
 * #include "DataFormats/TrackReco/interface/TrackFwd.h"
 */

#include "SimTracker/TrackHistory/interface/VertexClassifierByProxy.h"
#include "SimTracker/TrackHistory/interface/TrackClassifier.h"
//#include <SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h>
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
/*#include "CommonTools/Statistics/interface/ChiSquaredProbability.h"
 * #include "DataFormats/Math/interface/Vector.h"
 * #include "CommonTools/UtilAlgos/interface/TFileService.h"
 * #include "FWCore/ServiceRegistry/interface/Service.h"
 * #include "TROOT.h"
 * #include "Math/VectorUtil.h"
 * #include <TVector3.h>
 * #include <Math/GenVector/PxPyPzE4D.h>
 * #include <Math/GenVector/PxPyPzM4D.h>
 * #include "DataFormats/Math/interface/LorentzVector.h"
 */
#include <SimDataFormats/GeneratorProducts/interface/HepMCProduct.h>
#include <DataFormats/HepMCCandidate/interface/GenParticle.h> 
/*#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
 * #include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
 * #include <DataFormats/SiPixelCluster/interface/SiPixelCluster.h>  
 * #include "DataFormats/Common/interface/DetSetVector.h"
 * #include "DataFormats/SiPixelDigi/interface/PixelDigi.h"
 * #include "SimDataFormats/TrackerDigiSimLink/interface/PixelDigiSimLink.h"
 * #include "SimDataFormats/TrackerDigiSimLink/interface/StripDigiSimLink.h"
 * #include <DataFormats/Common/interface/DetSetNew.h>
 */
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "SimTracker/TrackHistory/interface/TrackHistory.h"
/*#include "DataFormats/Common/interface/Ref.h"
 * #include "DataFormats/DetId/interface/DetId.h"
 * #include "DataFormats/Math/interface/deltaR.h"
 */
//
// class decleration
//
using namespace reco;
using namespace std;
using namespace edm;
#include "DataFormats/GeometryVector/interface/VectorUtil.h"


namespace {
  
  bool isBHadron(int pdgId) {
    if ( (abs(pdgId) > 500 && abs(pdgId) < 600) || (abs(pdgId) > 5000 && abs(pdgId) < 6000)) return true;
    else return false;
  }
  
  bool isFromB(const HepMC::GenParticle* p, int max_depth) {
    if ( max_depth < 0 ) {
      std::cout << "WARNING! LOOP DETECTED!!" << endl;
      return false;
    }
    HepMC::GenVertex* prod_vertex = p->production_vertex();
    if (prod_vertex==0 || prod_vertex->particles_in_size() != 1)
      return false;
    else {
      HepMC::GenParticle* part_in = *prod_vertex->particles_in_const_begin();
      if ( isBHadron(part_in->pdg_id()) ) return true;
      else return isFromB (part_in, max_depth-1);
    }
  }
  
}

class BTracksAnalyzer : public edm::EDProducer
{
  
public:
  typedef math::XYZPoint 	Point;
  explicit BTracksAnalyzer(const edm::ParameterSet&);
  ~BTracksAnalyzer();
  
private:
  
  virtual void beginJob() ;		
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
  // Member data
  
  TrackClassifier trackclassifier_;	
  edm::InputTag tracks_;
  
};


BTracksAnalyzer::BTracksAnalyzer(const edm::ParameterSet& config) : 
trackclassifier_(config.getParameter<edm::ParameterSet>("trackConfig")),
tracks_(config.getUntrackedParameter<edm::InputTag> ( "trackInputTag" ))
{
  produces<reco::TrackCollection>("fakebTracks");
}

BTracksAnalyzer::~BTracksAnalyzer()
{  
}


void BTracksAnalyzer::produce(edm::Event& event, const edm::EventSetup& setup)
{
  
  trackclassifier_.newEvent(event, setup);
  
  edm::Handle<edm::View<reco::Track> > Tracks; 
  event.getByLabel(tracks_, Tracks);
  
  edm::Handle<edm::HepMCProduct> hmcp;
  event.getByLabel("generator", hmcp);
  
//   hmcp->GetEvent()->print();
  
  int ParticleSize = hmcp->GetEvent()->particles_size();
  
  auto_ptr<reco::TrackCollection> fakebTracks(new reco::TrackCollection);
  
  std::cout << tracks_.instance() << ":" << endl;
  
  for (unsigned int i=0; i<Tracks->size(); ++i) {
    reco::TrackBaseRef trkID((Tracks), i);
    trackclassifier_.evaluate(trkID);
    
    if (!trackclassifier_.is(TrackCategories::Fake) ) {
      TrackingParticleRef trackingparticle = trackclassifier_.history().simParticle();
//       std::cout << abs(trackingparticle->pdgId()) << " ";
      for (TrackingParticle::genp_iterator genp = trackingparticle->genParticle_begin(); genp != trackingparticle->genParticle_end(); ++genp) {
// 	std::cout << **genp << endl;
	if (!isFromB(&**genp, ParticleSize)) {
	  std::cout << "Track " << i << " is b fake." << endl;
	  fakebTracks->push_back(*trkID);
	}
	else {
	  std::cout << "Track " << i << " is b." << endl;
	}
// 	std::cout << (**genp).pdg_id() << " ";
      }
//       if ( (abs(trackingparticle->pdgId()) == 5) || (abs(trackingparticle->pdgId()) > 500 && abs(trackingparticle->pdgId()) < 600) || (abs(trackingparticle->pdgId()) > 5000 && abs(trackingparticle->pdgId()) < 6000) ) {
// 	std::cout << "Vertex " << tracks_.instance() << " Track " << i << " is b fake. " << endl;
// 	fakebTracks->push_back(*trkID);
//       }
    }
  }
  std::cout << endl;
  
  event.put(fakebTracks, "fakebTracks");
}

// ------------ method called once each job just before starting event loop  ------------
void 
BTracksAnalyzer::beginJob()
{
//   cout << "============NEW EVENT==========" << endl;
}

// ------------ method called once each job just after ending the event loop  ------------
void 
BTracksAnalyzer::endJob() {
  
  std::cout << std::endl;
  
} 

DEFINE_FWK_MODULE(BTracksAnalyzer);
