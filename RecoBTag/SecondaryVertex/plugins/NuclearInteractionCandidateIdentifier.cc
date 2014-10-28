#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/Handle.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "DataFormats/Common/interface/PtrVector.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"







class NuclearInteractionCandidateIdentifier : public edm::EDProducer {
    public:
	NuclearInteractionCandidateIdentifier(const edm::ParameterSet &params);


	virtual void produce(edm::Event &event, const edm::EventSetup &es);

    private:

	edm::EDGetTokenT<reco::VertexCollection> tokenPrimaryVertexCollection;
	edm::EDGetTokenT<edm::View<reco::VertexCompositePtrCandidate> > tokenSecondaryVertexCollection;
// 	edm::InputTag beamSpotCollection;
	edm::ParameterSet selectionCriteria;
};

NuclearInteractionCandidateIdentifier::NuclearInteractionCandidateIdentifier(const edm::ParameterSet &params) :
// 	beamSpotCollection           (params.getParameter<edm::InputTag>("beamSpot"))
	selectionCriteria(params.getParameter<edm::ParameterSet>("selection"))
{
	tokenPrimaryVertexCollection = consumes<reco::VertexCollection>(params.getParameter<edm::InputTag>("primaryVertices"));
	tokenSecondaryVertexCollection = consumes<edm::View<reco::VertexCompositePtrCandidate> >(params.getParameter<edm::InputTag>("secondaryVertices"));
	produces<std::vector<reco::VertexCompositePtrCandidate> >();
}


void NuclearInteractionCandidateIdentifier::produce(edm::Event &event, const edm::EventSetup &es)
{
// 	using namespace reco;
	
	typedef reco::Candidate::Point Point;
	typedef reco::Candidate::Vector Vector;

	edm::Handle<edm::View<reco::VertexCompositePtrCandidate> > secondaryVertices;
	event.getByToken(tokenSecondaryVertexCollection, secondaryVertices);
	
	edm::Handle<reco::VertexCollection> primaryVertices;
	event.getByToken(tokenPrimaryVertexCollection, primaryVertices);

	std::auto_ptr<std::vector<reco::VertexCompositePtrCandidate> > recoVertices(new std::vector<reco::VertexCompositePtrCandidate>);
	
	// 		const reco::Vertex &pv = (*primaryVertices)[0];
	
	// 		edm::Handle<BeamSpot> beamSpot;
	// 		event.getByLabel(beamSpotCollection, beamSpot);
	
	for(unsigned int ivtx=0; ivtx < secondaryVertices->size(); ivtx++){
		const reco::VertexCompositePtrCandidate & sv = (*secondaryVertices)[ivtx];
		// 			GlobalPoint ppv(pv.position().x(),pv.position().y(),pv.position().z());
		Point ssv(sv.vx(),sv.vy(),sv.vz());
		float mass=sv.mass();
		
		bool isNI = false;
		
		// check whether vertex is on detector material
		
		if (selectionCriteria.exists("position")){
			std::vector<double> layerCuts = selectionCriteria.getParameter<std::vector<double> >("position");
			
			for (unsigned int iLayer = 0; iLayer < layerCuts.size(); ++iLayer){
				if (iLayer % 2 == 0 && ssv.rho() >= layerCuts[iLayer]) isNI = true;
				if (iLayer % 2 != 0 && ssv.rho() > layerCuts[iLayer]) isNI = false;					
			}
			
// 			if (isNI)
// 				std::cout << "NI identified at rho = " << ssv.rho() << " in event " << event.id() << std::endl;
		}
		
		// if it is close to detector material, check for other criteria
		
		if (selectionCriteria.exists("maxZ") && isNI){
			float z = std::abs(ssv.z());
			double maxZ = selectionCriteria.getParameter<double>("maxZ");
			if (z > maxZ) isNI = false;
		}
		
		if (selectionCriteria.exists("minNctau") && isNI){
			const reco::Vertex &pv = (*primaryVertices)[0];
			Point ppv(pv.position().x(),pv.position().y(),pv.position().z());
			float pt=sv.pt();
			float gamma=pt/mass;
			Vector flightDir = ssv-ppv;
			float flightDistance2D = flightDir.rho();
			float Bctau = 0.05;		// c*tau for B hadron
			
			float nctau = flightDistance2D/(gamma*Bctau);	// number of c*taus for potential B hadron
			double minNctau = selectionCriteria.getParameter<double>("minNctau");
			if (nctau < minNctau) isNI = false;
		}
		
		if (selectionCriteria.exists("minMass") && isNI){
			double minMass = selectionCriteria.getParameter<double>("minMass");
			if (mass < minMass) isNI = false;				
		}
		if (selectionCriteria.exists("maxMass") && isNI){
			double maxMass = selectionCriteria.getParameter<double>("maxMass");
			if (mass > maxMass) isNI = false;				
		}
		if (selectionCriteria.exists("minNtracks") && isNI){
			int ntracks = sv.numberOfDaughters();
			int minNtracks = selectionCriteria.getParameter<int>("minNtracks");
			if (ntracks < minNtracks) isNI = false;				
		}
		if (selectionCriteria.exists("maxNtracks") && isNI){
			int ntracks = sv.numberOfDaughters();
			int maxNtracks = selectionCriteria.getParameter<int>("maxNtracks");
			if (ntracks > maxNtracks) isNI = false;				
		}
		
		if(isNI) {
			recoVertices->push_back(sv);
		}
		
	}	
	event.put(recoVertices);
}

DEFINE_FWK_MODULE(NuclearInteractionCandidateIdentifier);
