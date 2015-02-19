
#ifndef VertexCategorizer_h
#define VertexCategorizer_h

#include <set>

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimTracker/TrackAssociation/interface/TrackAssociatorBase.h"
#include "SimTracker/TrackHistory/interface/HistoryBase.h"

#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertexContainer.h"

#include "TrackingTools/PatternTools/interface/TSCPBuilderNoMaterial.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"

#include "SimTracker/VertexCategorization/interface/CategoryClasses.h"

#include "SimTracker/VertexCategorization/interface/TrackCategorizer.h"


// class OutputController
// {
// public:
// 	OutputController(bool print_it = false) : printOutput_(print_it) {}
// 	ostream& operator<< (ostream& os);

// private:
// 	bool printOutput_;
// };

// static OutputController msgLogger;


class TrackCategorizer;

class VertexCategorizer : HistoryBase
{

public:

	static bool messageOutput;	

    VertexCategorizer(edm::Event const & event, edm::EventSetup const & setup, edm::ParameterSet const & pset);

    virtual ~VertexCategorizer() {}

    void newEvent();

    void evaluate(edm::ParameterSet const & pset, TrackCategorizer & trackCategorizer)
    {
    	for (VertexMCInformation & vertInfo : analyzedVertices_)
    		evaluate(pset, vertInfo, trackCategorizer);
    }
    void evaluate (edm::ParameterSet const &, VertexMCInformation &, TrackCategorizer &);

    std::vector<VertexMCInformation> returnAnalyzedVertices() const
    {
    	return analyzedVertices_;
    }

private:

	static int vertexCount;

	// InputTags, strings, etc. from the parameter set

	const edm::InputTag trackProducer_;
	const edm::InputTag vertexProducer_;

    // double vertexClusteringSqDistance_;

	// Handles etc.

	edm::Handle<edm::View<reco::Track> > trackCollection_;
	edm::Handle<edm::View<reco::VertexCompositePtrCandidate> > vertexCollection_;

    // edm::ESHandle<ParticleDataTable> particleDataTable_;

	// private functions

	void reset()
	{
		analyzedVertices_.clear();
	}

	// other collections

	std::vector<VertexMCInformation> analyzedVertices_;

};

#endif
