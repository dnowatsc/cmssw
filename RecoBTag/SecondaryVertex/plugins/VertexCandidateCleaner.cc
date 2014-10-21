#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/Common/interface/PtrVector.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"


class VertexCandidateCleaner : public edm::EDProducer{
  public:
    explicit VertexCandidateCleaner(const edm::ParameterSet & iConfig);
    ~VertexCandidateCleaner();

    virtual void produce(edm::Event & iEvent, const edm::EventSetup& iSetup) override;
    virtual void endJob() override;

  private:
	  edm::EDGetTokenT<edm::View<reco::Candidate> > candSrcToken_;
	  edm::EDGetTokenT<edm::View<reco::VertexCompositePtrCandidate> > vetoSrcToken_;
};

VertexCandidateCleaner::VertexCandidateCleaner(const edm::ParameterSet & iConfig):
	candSrcToken_(consumes<edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>("srcTracks"))),
	vetoSrcToken_(consumes<edm::View<reco::VertexCompositePtrCandidate> >(iConfig.getParameter<edm::InputTag>("vetoVert")))
{
  produces<edm::PtrVector<reco::Candidate> >();	// maybe edm::PtrVector<reco::PFCandidate> ??
}

VertexCandidateCleaner::~VertexCandidateCleaner()
{
}

void
VertexCandidateCleaner::produce(edm::Event & iEvent, const edm::EventSetup & iSetup)
{
	using namespace edm;
	Handle<View<reco::Candidate> > cands;
	iEvent.getByToken(candSrcToken_, cands);
	Handle<View<reco::VertexCompositePtrCandidate> > vetos;
	iEvent.getByToken(vetoSrcToken_, vetos);

	std::auto_ptr<PtrVector<reco::Candidate> > result(new PtrVector<reco::Candidate>());
	std::set<reco::CandidatePtr> vetoedPtrs;
	for(size_t i = 0; i< vetos->size();  ++i) {
	for(size_t j=0,n=(*vetos)[i].numberOfDaughters(); j<n;j++ )    {
// 		reco::CandidatePtr c((*vetos)[i].daughter(j));
		vetoedPtrs.insert((*vetos)[i].daughterPtr(j));   
  }
  }
 for(size_t i = 0; i< cands->size();  ++i) {
    reco::CandidatePtr c =  cands->ptrAt(i);
    if(vetoedPtrs.find(c)==vetoedPtrs.end())
    {
      result->push_back(c);
    }
//     else std::cout << "Track removed!" << std::endl;
  }
  
//   std::cout << "  Original/cleaned pfCand size: " << cands->size() << " / " << result->size() << std::endl;
  
  iEvent.put(result);
}

void VertexCandidateCleaner::endJob()
{
}
DEFINE_FWK_MODULE(VertexCandidateCleaner);
