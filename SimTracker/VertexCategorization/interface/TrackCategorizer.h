
#ifndef TrackCategorizer_h
#define TrackCategorizer_h

#include <set>

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"

#include "SimTracker/TrackHistory/interface/VertexCategories.h"
// #include "SimTracker/TrackHistory/interface/VertexHistory.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimTracker/TrackAssociation/interface/TrackAssociatorBase.h"
#include "SimTracker/TrackHistory/interface/HistoryBase.h"
#include "SimTracker/TrackHistory/interface/Utils.h"
#include "SimTracker/TrackHistory/interface/TrackQuality.h"

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

#include "SimTracker/VertexCategorization/interface/VertexCategorizer2.h"

#include "SimTracker/VertexCategorization/interface/CategoryClasses.h"


namespace reco
{
	typedef edm::RefToBase<reco::Vertex> VertexBaseRef;
	typedef edm::AssociationMap<edm::OneToManyWithQuality <TrackingVertexCollection, edm::View<reco::Vertex>, double> > VertexSimToRecoCollection;
	typedef edm::AssociationMap<edm::OneToManyWithQuality <edm::View<reco::Vertex>, TrackingVertexCollection, double> > VertexRecoToSimCollection;
}

//! Get track history and classify it in function of their .
class TrackCategorizer : HistoryBase
{

public:

    //! Constructor by ParameterSet
    TrackCategorizer(edm::ParameterSet const & pset, reco::RecoToSimCollection const &, std::vector<VertexCategorizer2::GeneratedPrimaryVertex> const &);

    virtual ~TrackCategorizer() {}

    //! Pre-process event information (for accessing reconstraction information)
    virtual void newEvent(edm::Event const &, edm::EventSetup const &);

    //! Classify the RecoVertex in categories.
    void evaluate (TrackMCInformation &);

    void evaluate ()
    {
    	for (TrackMCInformation & trackMcInfo : analyzedTracks_)
    		evaluate(trackMcInfo);
    }

    std::vector<TrackMCInformation> returnAnalyzedTracks() const
    {
    	return analyzedTracks_;
    }
    //! Returns a reference to the vertex history used in the classification.
//     HistoryBase const & history() const
//     {
//         return tracer_;
//     }

private:

	reco::RecoToSimCollection const & recoToSimCollection_;
	std::vector<VertexCategorizer2::GeneratedPrimaryVertex> const & genpvs_;

	std::vector<TrackMCInformation> analyzedTracks_;
	
	void reset()
	{
		analyzedTracks_.clear();
	}

	bool bestMatchByMaxValue_;
	
	std::pair<double, double> calcPull(reco::TrackBaseRef const &, TrackingParticleRef const &);
	// void qualityInformation(HistoryBase::SimParticleTrail const &, reco::TrackBaseRef const &);
	
	// edm::InputTag trackProducer_;
	// edm::InputTag trackingTruth_;
	// std::string trackAssociator_;

	// HistoryBase tracer_;	/// switch to different/own History class? maybe can be optimized... => try to understand HistoryBase code better

	const edm::InputTag beamSpotLabel_;
	edm::ESHandle<MagneticField> magneticField_;
	edm::Handle<reco::BeamSpot> beamSpot_;

    double longLivedDecayLength_;
    double vertexClusteringSqDistance_;
	// unsigned int numberOfInnerLayers_;

    edm::ESHandle<ParticleDataTable> particleDataTable_;

    // Get geometrical information about the vertices
	int vertexInformationDist();
// 	int vertexInformationClust();
// 

    // Auxiliary function to get the generated primary vertex
	// bool checkBInHistory(HistoryBase::GenParticleTrail const &);
	// bool checkBInHistory(HistoryBase::SimParticleTrail const &);
	
 	// bool isFinalstateParticle(const HepMC::GenParticle *);
 	// bool isCharged(const HepMC::GenParticle *);
	// void genPrimaryVertices(edm::Handle<TrackingVertexCollection> const &);
	// int getLongLivedGenParticle(TrackingVertexRef const & tv) const;
	
	// TrackingParticleRef getSourceTrack(TrackingVertexRef const & evaluatedVertex) const;

};

#endif
