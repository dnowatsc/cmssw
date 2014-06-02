#ifndef VertexClassifierWeight_h
#define VertexClassifierWeight_h

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

#include "SimTracker/VertexHistory/interface/HistoryBaseOwn.h"

#include "CommonTools/RecoAlgos/interface/TrackingParticleSelector.h"

#include "DataFormats/GeometryVector/interface/VectorUtil.h"



//
// class decleration
//
using namespace reco;
using namespace std;
using namespace edm;

class VertexClassifierWeight : public HistoryBaseOwn
{
public:
    
    typedef std::map<TrackingVertexRef,std::pair<double, std::size_t> > MatchedTrackingVertexCollection;
    
  VertexClassifierWeight(const edm::ParameterSet &);
  ~VertexClassifierWeight();
  
  void newEvent(RefToBaseVector<Track>, RefVector<TrackingParticleCollection>, const edm::Event&, const EventSetup&, bool);
  void evaluate(const Vertex&);
  
  void reset()
  {
    HistoryBaseOwn::reset();
    thisProcessFlags_ = Flags(SigmaMinusDecay + 1, 0.);
    matches_.clear();
  }
  
  double getThisProcessWeight(Category category) const
  {
    return thisProcessFlags_[category];
  }
  
//   void processesAtSimulation(int &, TrackingVertexRef &);
//   
//   void processesAtGenerator(int &, TrackingVertexRef &);
  
//   TrackingVertexRef const & getTrackingVertex(MatchedTrackingVertexCollection & matchedTVcollection, double recoDaughterWeight = 1.) const		/// try using 'getTrackingVertex(...) const' instead of overload
//   {
//       const MatchedTrackingVertexCollection & mTVc = matchedTVcollection;
//       return getTrackingVertex(mTVc, recoDaughterWeight);
//   }
  
  std::pair<std::pair<float,int>, TrackingVertexRef> getBestTrackingVertex(const MatchedTrackingVertexCollection &, double recoDaughterWeight = 1.) const;	/// really need this function with optional argument?? probably not even working from outside
  
  std::pair<std::pair<float, int>, TrackingVertexRef> getBestTrackingVertex() const
  {
      const MatchedTrackingVertexCollection & mtvc = matches_;
      float rdw = recoDaughterWeight_;
      return getBestTrackingVertex(mtvc, rdw);
  }
  
  int getTrackingParticlePdgId(TrackingVertexRef const &) const;
  
  int getGenParticlePdgId(HepMC::GenVertex *) const;

  RecoToSimCollection const & getRecoToSim() { return trackRecoToSim_; }
  
  MatchedTrackingVertexCollection const & getMatchedTrackingVertices() { return matches_; }
  
  Flags const & getThisProcessFlags() { return thisProcessFlags_; }

  
private:
    float recoDaughterWeight_;
    
    edm::ParameterSet param_;
    
    TrackingParticleSelector selector_;
    
    RecoToSimCollection trackRecoToSim_;
    
    MatchedTrackingVertexCollection matches_;
    
    Flags thisProcessFlags_;
  
  
};

#endif