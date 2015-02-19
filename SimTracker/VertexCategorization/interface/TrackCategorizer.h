
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

#include "SimTracker/VertexCategorization/interface/VertexCategorizer.h"

#include "SimTracker/VertexCategorization/interface/CategoryClasses.h"


class TrackCategorizer : HistoryBase
{

public:

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

    TrackCategorizer(edm::Event const &, edm::EventSetup const &, edm::ParameterSet const &);

    virtual ~TrackCategorizer() {}

    virtual void newTrackCollection(edm::RefToBaseVector<reco::Track> const &);

    virtual void evaluate (TrackMCInformation &);

    virtual void evaluate ()
    {
    	for (TrackMCInformation & trackMcInfo : analyzedTracks_)
    		evaluate(trackMcInfo);
    }

    std::vector<TrackMCInformation> returnAnalyzedTracks() const
    {
    	return analyzedTracks_;
    }

private:

	static int trackCount;

	// references to event and setup
	const edm::Event & event_;
	const edm::EventSetup & eventSetup_;



	// InputTags, strings, etc. from the parameter set

	const edm::InputTag beamSpotLabel_;
	const edm::InputTag trackingTruth_;
	const edm::InputTag candidatesLabel_;
	const edm::InputTag hepMCLabel_;

	const std::string trackAssociatorProducer_;

	// double longLivedDecayLength_;
    double vertexClusteringSqDistance_;
	bool bestMatchByMaxValue_;



	// Handles
	
	edm::Handle<reco::BeamSpot> beamSpot_;
	edm::Handle<TrackingParticleCollection>  TPCollection_;
	edm::Handle<edm::HepMCProduct> mcInformation_;
	edm::Handle<edm::View<reco::Candidate> > candidates_;

	edm::ESHandle<TrackAssociatorBase> trackAssociator_;
	edm::ESHandle<MagneticField> magneticField_;
    edm::ESHandle<ParticleDataTable> particleDataTable_;

	// unsigned int numberOfInnerLayers_;



	// private functions

	void reset()
	{
		genpvs_.clear();
		trackRecoToSim_.clear();
		tpRefVec_.clear();
		analyzedTracks_.clear();
	}

	void genPrimaryVertices();

	int vertexInformationDist(HistoryBase::GenParticleTrail const &, HistoryBase::SimParticleTrail const &) const;

	std::pair<double, double> calcPull(reco::TrackBaseRef const &, TrackingParticleRef const &) const;

	bool isFinalstateParticle(const HepMC::GenParticle *);
 	bool isCharged(const HepMC::GenParticle *);

	TrackingParticleRef getSourceTrack(TrackingVertexRef const & evaluatedVertex) const;

	int getLongLivedGenParticle(TrackingVertexRef const & tv) const;
	bool checkBInHistory(HistoryBase::GenParticleTrail const &);
	bool checkBInHistory(HistoryBase::SimParticleTrail const &);




	// other collections

	reco::RecoToSimCollection trackRecoToSim_;
	edm::RefVector<TrackingParticleCollection> tpRefVec_;
	std::vector<GeneratedPrimaryVertex> genpvs_;

	std::vector<TrackMCInformation> analyzedTracks_;

};

#endif
