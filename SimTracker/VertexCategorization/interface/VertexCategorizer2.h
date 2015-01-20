
#ifndef VertexCategorizer2_h
#define VertexCategorizer2_h

#include <set>

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"

// #include "SimTracker/TrackHistory/interface/VertexCategories.h"
// #include "SimTracker/TrackHistory/interface/VertexHistory.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimTracker/TrackAssociation/interface/TrackAssociatorBase.h"
#include "SimTracker/TrackHistory/interface/HistoryBase.h"
// #include "SimTracker/TrackHistory/interface/Utils.h"
// #include "SimTracker/TrackHistory/interface/TrackQuality.h"

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

namespace reco
{
	typedef edm::RefToBase<reco::Vertex> VertexBaseRef;
	typedef edm::AssociationMap<edm::OneToManyWithQuality <TrackingVertexCollection, edm::View<reco::Vertex>, double> > VertexSimToRecoCollection;
	typedef edm::AssociationMap<edm::OneToManyWithQuality <edm::View<reco::Vertex>, TrackingVertexCollection, double> > VertexRecoToSimCollection;
}

//! Get track history and classify it in function of their .
class VertexCategorizer2 : HistoryBase
{

public:

    //! Type to the associate category
    // typedef VertexCategories Categories;
	
    //! Constructor by ParameterSet
    VertexCategorizer2(edm::ParameterSet const & pset);

    virtual ~VertexCategorizer2() {}

    //! Pre-process event information (for accessing reconstraction information)
    void newEvent(edm::Event const &, edm::EventSetup const &);

    void evaluate(edm::ParameterSet const & pset)
    {
    	for (VertexMCInformation & vertInfo : analyzedVertices_)
    		evaluate(pset, vertInfo);
    }
    //! Classify the RecoVertex in categories.
    void evaluate (edm::ParameterSet const &, VertexMCInformation &);

    std::vector<VertexMCInformation> returnAnalyzedVertices() const
    {
    	return analyzedVertices_;
    }

    //! Classify the TrackingVertex in categories.
	// VertexCategorizer2 const & evaluate (TrackingVertexRef const &);

    //! Classify the RecoVertex in categories.
    // VertexCategorizer2 const & evaluate (reco::VertexRef const & vertex)
    // {
    //     return evaluate( reco::VertexBaseRef(vertex) );
    // }

    //! Returns a reference to the vertex history used in the classification.
	// HistoryBase const & history() const
	// {
	//     return tracer_;
	// }

    /// MODIFICATION
 	// double weight(Category category) const
 	// {
	// 	return flags_[category];
	// }
	
	// std::vector<double> const & flags() const
	// {
	// 	return flags_;
	// }

private:

	std::vector<VertexMCInformation> analyzedVertices_;
	
	// TrackQuality quality_;
	
	void reset()
	{
		tpRefVec_.clear();
		analyzedVertices_.clear();
	}
	
	// bool checkIsBad(reco::TrackBaseRef const &, TrackingParticleRef const &);
	// void qualityInformation(HistoryBase::SimParticleTrail const &, reco::TrackBaseRef const &);

	edm::InputTag trackProducer_;
	edm::InputTag vertexProducer_;
	edm::InputTag trackingTruth_;
	const edm::InputTag hepMCLabel_;
	std::string trackAssociatorProducer_;

	edm::Handle<edm::View<reco::Track> > trackCollection_;
	edm::Handle<edm::View<reco::VertexCompositePtrCandidate> > vertexCollection_;
	edm::Handle<TrackingParticleCollection>  TPCollection_;
	edm::ESHandle<TrackAssociatorBase> trackAssociator_;
	edm::Handle<edm::HepMCProduct> mcInformation_;
	
	edm::RefVector<TrackingParticleCollection> tpRefVec_;

	void genPrimaryVertices();

	//! Auxiliary class holding simulated primary vertices
    struct GeneratedPrimaryVertex
    {
        GeneratedPrimaryVertex(double x1,double y1,double z1): x(x1), y(y1), z(z1), ptsq(0), nGenTrk(0) {}

        bool operator< ( GeneratedPrimaryVertex const & reference) const
        {
            return ptsq < reference.ptsq;
        }

        double x, y, z;
        double ptsq;
        int nGenTrk;

        HepMC::FourVector ptot;

        std::vector<int> finalstateParticles;
        std::vector<int> simTrackIndex;
        std::vector<int> genVertex;
    };

    std::vector<GeneratedPrimaryVertex> genpvs_;

    friend class TrackCategorizer;

	// bool enableRecoToSim_;
	// bool bestMatchByMaxValue_;
	// double badPull_;

	// HistoryBase tracer_;	/// switch to different/own History class? maybe can be optimized... => try to understand HistoryBase code better

 //    const edm::InputTag hepMCLabel_;
	// const edm::InputTag beamSpotLabel_;
	// edm::ESHandle<MagneticField> magneticField_;
	// edm::Handle<reco::BeamSpot> beamSpot_;

 //    double longLivedDecayLength_;
    double vertexClusteringSqDistance_;
	// unsigned int numberOfInnerLayers_;

 //    edm::Handle<edm::HepMCProduct> mcInformation_;

    edm::ESHandle<ParticleDataTable> particleDataTable_;

	// //! Get reconstruction information
	// void reconstructionInformation(reco::TrackBaseRef const &);
 
	// //! Get all the information related to the simulation details
	// void simulationInformation();
 
	// //! Get all the information related to decay process
	// void processesAtGenerator();
 
	// //! Get information about conversion and other interactions
	// void processesAtSimulation();
 
    //! Get geometrical information about the vertices
	// int vertexInformationDist(HistoryBase const &);
	// int vertexInformationClust();

    //! Auxiliary class holding simulated primary vertices
    // struct GeneratedPrimaryVertex
    // {
    //     GeneratedPrimaryVertex(double x1,double y1,double z1): x(x1), y(y1), z(z1), ptsq(0), nGenTrk(0) {}

    //     bool operator< ( GeneratedPrimaryVertex const & reference) const
    //     {
    //         return ptsq < reference.ptsq;
    //     }

    //     double x, y, z;
    //     double ptsq;
    //     int nGenTrk;

    //     HepMC::FourVector ptot;

    //     std::vector<int> finalstateParticles;
    //     std::vector<int> simTrackIndex;
    //     std::vector<int> genVertex;
    // };

    // std::vector<GeneratedPrimaryVertex> genpvs_;

    // Auxiliary function to get the generated primary vertex
	// bool checkBInHistory(HistoryBase::GenParticleTrail const &);
	// bool checkBInHistory(HistoryBase::SimParticleTrail const &);
	
 	bool isFinalstateParticle(const HepMC::GenParticle *);
 	bool isCharged(const HepMC::GenParticle *);
	// void genPrimaryVertices(edm::Handle<TrackingVertexCollection> const &);
	// int getLongLivedGenParticle(TrackingVertexRef const & tv) const;
	
	// TrackingParticleRef getSourceTrack(TrackingVertexRef const & evaluatedVertex) const;

};

#endif
