#ifndef HistoryBaseOwn_h
#define HistoryBaseOwn_h

#include <set>

#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"

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

//! Base class to all the history types.
class HistoryBaseOwn
{

public:
  
  struct G4
  {
    enum Process
    {
      Undefined = 0,
      Unknown,
      Primary,
      Hadronic,
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
      MuNucl	    
    };
    
  };
  
  enum Category
  {
    Fake = 0,
    HadronicProcess,
    UnknownProcess,
    UndefinedProcess,
    GeantPrimaryProcess,
    DecayProcess,
    ComptonProcess,
    AnnihilationProcess,
    EIoniProcess,
    HIoniProcess,
    MuIoniProcess,
    PhotonProcess,
    MuPairProdProcess,
    ConversionsProcess,
    EBremProcess,
    SynchrotronRadiationProcess,
    MuBremProcess,
    MuNuclProcess,
    PrimaryProcess,
    ProtonDecay,
    BWeakDecay,
    CWeakDecay,
    FromBWeakDecayMuon,
    FromCWeakDecayMuon,
    ChargePionDecay,
    ChargeKaonDecay,
    TauDecay,
    KsDecay,
    LambdaDecay,
    JpsiDecay,
    XiDecay,
    SigmaPlusDecay,
    SigmaMinusDecay    
  };

    //! GenParticle trail type.
    typedef std::vector<const HepMC::GenParticle *> GenParticleTrail;

    //! GenVertex trail type.
    typedef std::vector<const HepMC::GenVertex *> GenVertexTrail;

    //! GenVertex trail helper type.
    typedef std::set<const HepMC::GenVertex *> GenVertexTrailHelper;

    //! SimParticle trail type.
    typedef std::vector<TrackingParticleRef> SimParticleTrail;

    //! SimVertex trail type.
    typedef std::vector<TrackingVertexRef> SimVertexTrail;
    
    typedef std::vector<double> Flags;

    // Default constructor
    HistoryBaseOwn()
    {
        // Default depth
        status_ = 1;
	
	debugMsg_ = false;
    }
    
    void status(int d)
    {
        status_ = d;
    }
    
    void newEvent(const edm::EventSetup& setup) { setup.getData(particleDataTable_); }
    
    void reset()
    {
      historyFlags_ = Flags(SigmaMinusDecay + 1, 0.);
    }

    //! Return all the simulated vertices in the history.
//     SimVertexTrail const & simVertexTrail() const
//     {
//         return simVertexTrail_;
//     }
// 
//     //! Return all the simulated particle in the history.
//     SimParticleTrail const & simParticleTrail() const
//     {
//         return simParticleTrail_;
//     }
// 
//     //! Return all generated vertex in the history.
//     GenVertexTrail const & genVertexTrail() const
//     {
//         return genVertexTrail_;
//     }
// 
//     //! Return all generated particle in the history.
//     GenParticleTrail const & genParticleTrail() const
//     {
//         return genParticleTrail_;
//     }
// 
//     //! Return the initial tracking particle from the history.
//     const TrackingParticleRef & simParticle() const
//     {
//         return simParticleTrail_[0];
//     }
// 
//     //! Return the initial tracking vertex from the history.
//     const TrackingVertexRef & simVertex() const
//     {
//         return simVertexTrail_[0];
//     }

//     const HepMC::GenParticle * genParticle() const
//     {
//         if ( genParticleTrail_.empty() ) return 0;
//         return genParticleTrail_[genParticleTrail_.size()-1];
//     }
    
    void debugMsg(bool debug) { debugMsg_ = debug; }
    
    double getHistoryWeight(Category category) const
    {
      return historyFlags_[category];
    }
    
    Flags const & getHistoryProcessFlags() { return historyFlags_; }
    
    void flagProcess(Flags &, int, double, bool cumulative = true);
    
    TrackingParticleRef getSourceTrack(TrackingVertexRef const &) const;	/// later try 'TrackingParticleRef getSourceTrack(TrackingVertexRef &) const;' without overloading the function
    
    TrackingParticleRef getSourceTrack(TrackingVertexRef & trackVertRef) const
    {
      TrackingVertexRef const & inputVertRef = trackVertRef;
      return getSourceTrack(inputVertRef);
    }
    
    double distanceToPrevVertex(HepMC::GenVertex const *, HepMC::GenVertex const *) const;
    
    double distanceToPrevVertex(HepMC::GenVertex *thisVertex, HepMC::GenVertex *motherVertex) const
    {
	HepMC::GenVertex const * thisConstVert = thisVertex;
	HepMC::GenVertex const * motherConstVert = motherVertex;
	return distanceToPrevVertex(thisConstVert, motherConstVert);
    }

protected:
    
    bool debugMsg_;
    
    Flags historyFlags_;
    
    edm::ESHandle<ParticleDataTable> particleDataTable_;

    // History cointainers
//     GenVertexTrail genVertexTrail_;
//     GenParticleTrail genParticleTrail_;
//     SimVertexTrail simVertexTrail_;
    SimParticleTrail simParticleTrail_;	// only to check for looping tracks

    // Helper function to speedup search
    GenVertexTrailHelper genVertexTrailHelper_;

    //! Evaluate track history using a TrackingParticleRef.
    
    bool evaluate(TrackingParticleRef const & tpr, Flags & flags, int histSteps, double weight)
    {
	histSteps_ = histSteps;
        resetTrails(tpr);
	return traceSimHistory(tpr, flags, 0, weight);
    }

    //! Evaluate track history using a TrackingParticleRef.
    
    bool evaluate(TrackingVertexRef const & tvr, Flags & flags, int histSteps, double weight)
    {
	histSteps_ = histSteps;
        resetTrails();
        return traceSimHistory(tvr, flags, 0, weight);
    }

private:
    
    int status_;

    int histSteps_;
    
//     int historySteps_;

    //! Trace all the simulated information for a given reference to a TrackingParticle.
    bool traceSimHistory (TrackingParticleRef const &, Flags &, int, double);

    //! Trace all the simulated information for a given reference to a TrackingVertex.
    bool traceSimHistory (TrackingVertexRef const &, Flags &, int, double);

    //! Trace all the simulated information for a given pointer to a GenParticle.
    bool traceGenHistory (HepMC::GenParticle const *, Flags &, int, double);

    //! Trace all the simulated information for a given pointer to a GenVertex.
    bool traceGenHistory (HepMC::GenVertex const *, Flags &, int, double);

    //! Reset trail functions.
    void resetTrails()
    {
        simParticleTrail_.clear();
//         simVertexTrail_.clear();
//         genVertexTrail_.clear();
//         genParticleTrail_.clear();
        genVertexTrailHelper_.clear();
    }

    void resetTrails(TrackingParticleRef tpr)
    {
        resetTrails();
        simParticleTrail_.push_back(tpr);
    }
};

#endif
