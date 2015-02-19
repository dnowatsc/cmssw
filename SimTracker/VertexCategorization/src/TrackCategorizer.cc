/*
 *  TrackCategorizer.C
 */

#include <math.h>
#include <cstdlib>
#include <iostream>

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "HepPDT/ParticleID.hh"

#include "SimTracker/VertexCategorization/interface/TrackCategorizer.h"

#include "SimTracker/Records/interface/TrackAssociatorRecord.h"

#include "SimTracker/TrackHistory/interface/Utils.h"

int TrackCategorizer::trackCount = 0;


namespace{
	void fillVec(std::vector<int> &, int);
	
	void PUVerticesFinder(edm::Handle<TrackingVertexCollection> const & tvCollection){
		int counter1 = 0;
		int counter2 = 0;
		int counter3 = 0;
		std::vector<int> eventNo;
		std::vector<int> bunchCrossingNo;
		int count = 0;
		std::cout << "BunchCrossing / EventNo:" << std::endl;
		for (TrackingVertexCollection::const_iterator iTV = tvCollection->begin(); iTV < tvCollection->end(); ++iTV, ++count){
			bool closeBeam = (iTV->position().rho() < 1);
			int bc = iTV->eventId().bunchCrossing();
			int en = iTV->eventId().event();
			fillVec(eventNo, en);
			fillVec(bunchCrossingNo, bc);
			if (!iTV->nSourceTracks() && !iTV->nGenVertices() && closeBeam){
				counter1++;
				// std::cout << "  Vertex found at (x,y,z): " << iTV->position().x() << " " << iTV->position().y() << " " << iTV->position().z() << std::endl;
			}
			if (!iTV->nSourceTracks() && !iTV->nGenVertices()) counter2++;
			if (closeBeam) counter3++;
			if (count < 200000) std::cout << bc << " " << en << std::endl;
		}
		std::cout << std::endl << "Found PU vertices close to beam / Found PU vertices / Vertices close to beam / TVCollection size: " << counter1 << " " << counter2 << " " << counter3 << " " << tvCollection->size() << std::endl;
		std::cout << "Event number countings of TVs: ";
		for (unsigned int i = 0; i < eventNo.size(); ++i) std::cout << eventNo[i] << " ";
		std::cout << std::endl << "BunchCrossing number countings of TVs: ";
		for (unsigned int i = 0; i < bunchCrossingNo.size(); ++i) std::cout << bunchCrossingNo[i] << " ";
		std::cout << std::endl << std::endl;
	}
	
	void fillVec(std::vector<int> & vec, int i){
		if ((int)vec.size() < i+1){
			vec.push_back(0);
			fillVec(vec, i);
		} else {
			vec[i]++;
		}
	}
	
	double distanceToPrevGenVertex (HepMC::GenVertex const * thisVertex, HepMC::GenVertex const * motherVertex)
	{
		HepMC::ThreeVector thisVertPos = thisVertex->point3d();
		HepMC::ThreeVector motherVertPos = motherVertex->point3d();
		HepMC::ThreeVector flightVector;
		flightVector.set(thisVertPos.x()-motherVertPos.x(), thisVertPos.y()-motherVertPos.y(), thisVertPos.z()-motherVertPos.z());
		double travelDistance = flightVector.r(); //! in mm
		
		return travelDistance;
	}
}


TrackCategorizer::TrackCategorizer(edm::Event const & event, edm::EventSetup const & setup, edm::ParameterSet const & config) :
        // quality_(config),	
		HistoryBase(),
		event_(event),
		eventSetup_(setup),
        beamSpotLabel_( config.getUntrackedParameter<edm::InputTag>("beamSpot") ),
        trackingTruth_( config.getUntrackedParameter<edm::InputTag>("trackingTruth") ),
        candidatesLabel_( config.getUntrackedParameter<edm::InputTag>("candidates") ),
        hepMCLabel_(config.getUntrackedParameter<edm::InputTag>("hepMC") ),
        trackAssociatorProducer_( config.getUntrackedParameter<std::string>("trackAssociator") )
{
	reset();

	HistoryBase::depth(-2);
	bestMatchByMaxValue_ = config.getUntrackedParameter<bool>("bestMatchByMaxValue");

    // Set the minimum decay length for detecting long decays
	// longLivedDecayLength_ = config.getUntrackedParameter<double>("longLivedDecayLength");

    // Set the distance for clustering vertices
	float vertexClusteringDistance = config.getUntrackedParameter<double>("vertexClusteringDistance");
	vertexClusteringSqDistance_ = vertexClusteringDistance * vertexClusteringDistance;

	
	// Set the number of innermost layers to check for bad hits
	// numberOfInnerLayers_ = config.getUntrackedParameter<unsigned int>("numberOfInnerLayers");
	
	setup.get<IdealMagneticFieldRecord>().get(magneticField_);

    setup.getData(particleDataTable_);
	
	event.getByLabel(beamSpotLabel_, beamSpot_);

	event.getByLabel(hepMCLabel_, mcInformation_);

	event.getByLabel(candidatesLabel_, candidates_);

	setup.get<TrackAssociatorRecord>().get(trackAssociatorProducer_, trackAssociator_);

	genPrimaryVertices();

	// create a RefVector from the TrackingParticle collection; needed for the qTABH to work with a RefToBaseVector<reco::Track>

	event.getByLabel(trackingTruth_, TPCollection_);

	for (size_t iTP = 0; iTP < TPCollection_->size(); ++iTP)
	{
		edm::Ref<TrackingParticleCollection> tpRef(TPCollection_.product(), iTP);
		tpRefVec_.push_back(tpRef);
	}

}

void TrackCategorizer::newTrackCollection(edm::RefToBaseVector<reco::Track> const & trackRefs)
{
	trackCount = 0;
	analyzedTracks_.clear();
	trackRecoToSim_.clear();
	trackRecoToSim_ = trackAssociator_->associateRecoToSim(trackRefs, tpRefVec_, &event_, &eventSetup_);

	for (reco::RecoToSimCollection::const_iterator iMatch = trackRecoToSim_.begin(); iMatch != trackRecoToSim_.end(); ++iMatch)
	{
		TrackMCInformation newTrackMCInfo;
		newTrackMCInfo.recoTrackRef = iMatch->key;
		for (size_t iCand = 0; iCand != candidates_->size(); ++iCand )
		{
			const reco::Track * candTrack = candidates_->refAt(iCand)->bestTrack();
			if (&(*newTrackMCInfo.recoTrackRef) == candTrack)
				newTrackMCInfo.candRef = candidates_->refAt(iCand);
		}
		analyzedTracks_.push_back(newTrackMCInfo);
	}
}


void TrackCategorizer::evaluate (TrackMCInformation & trackMcInfo)
{

	if (VertexCategorizer::messageOutput)
    {
    	std::cout << "    Track " << trackCount++ << ": " << std::endl;
        // std::cout << "      Reco mother position 1: " << trackMcInfo.recoTrackRef->vertex().x() << "|" << trackMcInfo.recoTrackRef->vertex().y() << "|" << trackMcInfo.recoTrackRef->vertex().z() << std::endl;
        std::cout << "      Reco mother rho: " << trackMcInfo.recoTrackRef->vertex().rho() << std::endl;
    }
    
    // std::cout << "      Reco mother position 2: " << trackMcInfo.candRef->mother()->vx() << "|" << trackMcInfo.candRef->mother()->vy() << "|" << trackMcInfo.candRef->mother()->vz() << std::endl;



    trackMcInfo.numberMatches = trackRecoToSim_[trackMcInfo.recoTrackRef].size();

    std::pair<TrackingParticleRef, double> result =  match(trackMcInfo.recoTrackRef, trackRecoToSim_, bestMatchByMaxValue_);
	
	TrackingParticleRef tpr( result.first );

	if (VertexCategorizer::messageOutput)
	{
		std::cout << "      Number of matches: " << trackMcInfo.numberMatches << std::endl;
		std::cout << "      True mother position / matching quality: " << tpr->parentVertex()->position().rho() << " / " << result.second << std::endl;
	}

	trackMcInfo.matchingQuality = result.second;

	if (!tpr.isNull()){
		// 	std::cout << tpr->parentVertex()->position().x() << "/" << tpr->parentVertex()->position().y() << "/" << tpr->parentVertex()->position().z();

		HistoryBase::evaluate(tpr);
		trackMcInfo.vertexType = vertexInformationDist(this->genParticleTrail(), this->simParticleTrail());
		if (VertexCategorizer::messageOutput)
			std::cout << "      Mother vertex type: " << trackMcInfo.vertexType << std::endl;


		// 	std::cout << " | " << vertexKindDist;

		std::pair<double, double> pull = calcPull(trackMcInfo.recoTrackRef, tpr);
		trackMcInfo.dxyPull = pull.first;
		trackMcInfo.dzPull = pull.second;

		trackMcInfo.geantProcessType = tpr->parentVertex()->g4Vertices_begin()->processType();
		trackMcInfo.cmsProcessType = mapG4toCMSProcType(trackMcInfo.geantProcessType);

		int pdgid = 0;

		if (tpr->parentVertex()->nSourceTracks()){

			TrackingParticleRef const & selectedTrack = getSourceTrack(tpr->parentVertex());

			if (!selectedTrack.isNull())
				pdgid = selectedTrack->pdgId();

		} else if (tpr->parentVertex()->nGenVertices()){
			pdgid = getLongLivedGenParticle(tpr->parentVertex());
		}

		trackMcInfo.motherParticleID = std::abs(pdgid);

		trackMcInfo.bInHistory = (checkBInHistory(this->genParticleTrail()) || checkBInHistory(this->simParticleTrail()));

			
		
		}

		else {
		}
	
}

bool TrackCategorizer::isFinalstateParticle(const HepMC::GenParticle * p)
{
    return !p->end_vertex() && p->status() == 1;
}


bool TrackCategorizer::isCharged(const HepMC::GenParticle * p)
{
    const ParticleData * part = particleDataTable_->particle( p->pdg_id() );
    if (part)
        return part->charge()!=0;
    else
    {
        // the new/improved particle table doesn't know anti-particles
        return  particleDataTable_->particle( -p->pdg_id() ) != 0;
    }
}

void TrackCategorizer::genPrimaryVertices()

{
    genpvs_.clear();

    const HepMC::GenEvent * event = mcInformation_->GetEvent();

    if (event)
    {
        int idx = 0;

        // Loop over the different GenVertex
        for ( HepMC::GenEvent::vertex_const_iterator ivertex = event->vertices_begin(); ivertex != event->vertices_end(); ++ivertex )
        {
            bool hasParentVertex = false;

            // Loop over the parents looking to see if they are coming from a production vertex
            for (
                HepMC::GenVertex::particle_iterator iparent = (*ivertex)->particles_begin(HepMC::parents);
                iparent != (*ivertex)->particles_end(HepMC::parents);
                ++iparent
            )
                if ( (*iparent)->production_vertex() )
                {
                    hasParentVertex = true;
                    break;
                }

            // Reject those vertices with parent vertices
            if (hasParentVertex) continue;

            // Get the position of the vertex
            HepMC::FourVector pos = (*ivertex)->position();

            double const mm = 0.1;

          GeneratedPrimaryVertex pv(pos.x()*mm, pos.y()*mm, pos.z()*mm);

            std::vector<GeneratedPrimaryVertex>::iterator ientry = genpvs_.begin();

            // Search for a VERY close vertex in the list
            for (; ientry != genpvs_.end(); ++ientry)
            {
                double distance = sqrt( pow(pv.x - ientry->x, 2) + pow(pv.y - ientry->y, 2) + pow(pv.z - ientry->z, 2) );
                if ( distance < vertexClusteringSqDistance_ )
                    break;
            }

            // Check if there is not a VERY close vertex and added to the list
            if (ientry == genpvs_.end())
                ientry = genpvs_.insert(ientry,pv);

            // Add the vertex barcodes to the new or existent vertices
            ientry->genVertex.push_back((*ivertex)->barcode());

            // Collect final state descendants
            for (
                HepMC::GenVertex::particle_iterator idecendants  = (*ivertex)->particles_begin(HepMC::descendants);
                idecendants != (*ivertex)->particles_end(HepMC::descendants);
                ++idecendants
            )
            {
                if (isFinalstateParticle(*idecendants))
                    if ( find(ientry->finalstateParticles.begin(), ientry->finalstateParticles.end(), (*idecendants)->barcode()) == ientry->finalstateParticles.end() )
                    {
                        ientry->finalstateParticles.push_back((*idecendants)->barcode());
                        HepMC::FourVector m = (*idecendants)->momentum();

                        ientry->ptot.setPx(ientry->ptot.px() + m.px());
                        ientry->ptot.setPy(ientry->ptot.py() + m.py());
                        ientry->ptot.setPz(ientry->ptot.pz() + m.pz());
                        ientry->ptot.setE(ientry->ptot.e() + m.e());
                        ientry->ptsq += m.perp() * m.perp();

                        if ( m.perp() > 0.8 && std::abs(m.pseudoRapidity()) < 2.5 && isCharged(*idecendants) ) ientry->nGenTrk++;
                    }
            }
            idx++;
        }
    }

    std::sort(genpvs_.begin(), genpvs_.end());
}


bool TrackCategorizer::checkBInHistory(HistoryBase::GenParticleTrail const & genParticleTrail)
{
    // Loop over the generated vertices
    for (HistoryBase::GenParticleTrail::const_iterator iTrack = genParticleTrail.begin(); iTrack != genParticleTrail.end(); ++iTrack)
    {
        // Get the pointer to the vertex by removing the const-ness (no const methos in HepMC::GenVertex)
        HepMC::GenParticle const * particle = *iTrack;
		
		// Collect the pdgid of the parent
		int pdgid = std::abs(particle->pdg_id());
		// Get particle type
		HepPDT::ParticleID particleID(pdgid);
		
		// Check if the particle type is valid one
		if (particleID.isValid())
		{
			if (particleID.hasBottom()) return true;
		}
    }
    
    return false;
}


bool TrackCategorizer::checkBInHistory(HistoryBase::SimParticleTrail const & simParticleTrail)
{

	for (HistoryBase::SimParticleTrail::const_iterator iTrack = simParticleTrail.begin(); iTrack != simParticleTrail.end(); ++iTrack)
    {
        // pdgid of the real source parent vertex
		
		int pdgid = std::abs((*iTrack)->pdgId());
		
		// Get particle type
		HepPDT::ParticleID particleID(pdgid);
		// Check if the particle type is valid one
		if (particleID.isValid())
		{
			if (particleID.hasBottom()) return true;
		}
		
	}
	
	return false;
}


int TrackCategorizer::vertexInformationDist(HistoryBase::GenParticleTrail const & genParticleTrail, HistoryBase::SimParticleTrail const & simParticleTrail) const
{
	if (genpvs_.size() == 0){
		std::cout << "NO PV FOUND!" << std::endl;
		return -1;
	}
	// Get the main primary vertex from the list
	GeneratedPrimaryVertex const & genpv = genpvs_.back();
	
	// Vertex counter
	int counter = 0;
	
	// Unit transformation from mm to cm
	double const mm = 0.1;
	
	double oldX = genpv.x;
	double oldY = genpv.y;
	double oldZ = genpv.z;
	
	// Loop over the generated particles
	for (
		HistoryBase::GenParticleTrail::const_reverse_iterator iparticle = genParticleTrail.rbegin();
	iparticle != genParticleTrail.rend();
	++iparticle
	)
	{
		// Look for those with production vertex
		HepMC::GenVertex const * parent = (*iparticle)->production_vertex();
		if (parent)
		{
			HepMC::ThreeVector p = parent->point3d();
			
			double distance2   = pow(p.x() * mm - genpv.x, 2) + pow(p.y() * mm - genpv.y, 2) + pow(p.z() * mm - genpv.z, 2);
			double difference2 = pow(p.x() * mm - oldX, 2)    + pow(p.y() * mm - oldY, 2)    + pow(p.z() * mm - oldZ, 2);
			
			// std::cout << "Distance2 : " << distance2 << " (" << p.x() * mm << "," << p.y() * mm << "," << p.z() * mm << ")" << std::endl;
			// std::cout << "Difference2 : " << difference2 << std::endl;
			
			if ( difference2 > vertexClusteringSqDistance_ )
			{
				if ( distance2 > vertexClusteringSqDistance_ ) counter++;
				oldX = p.x() * mm;
				oldY = p.y() * mm;
				oldZ = p.z() * mm;
			}
		}
	}
	
	// HistoryBase::SimParticleTrail & simParticleTrail = const_cast<HistoryBase::SimParticleTrail &> (tracer_.simParticleTrail());
	
	// Loop over the generated particles
	for (
		HistoryBase::SimParticleTrail::const_reverse_iterator iparticle = simParticleTrail.rbegin();
	iparticle != simParticleTrail.rend();
	++iparticle
	)
	{
		// Look for those with production vertex
		TrackingParticle::Point p = (*iparticle)->vertex();
		
		double distance2   = pow(p.x() - genpv.x, 2) + pow(p.y() - genpv.y, 2) + pow(p.z() - genpv.z, 2);
		double difference2 = pow(p.x() - oldX, 2)    + pow(p.y() - oldY, 2)    + pow(p.z() - oldZ, 2);
		
		// std::cout << "Distance2 : " << distance2 << " (" << p.x() << "," << p.y() << "," << p.z() << ")" << std::endl;
		// std::cout << "Difference2 : " << difference2 << std::endl;
		
		if ( difference2 > vertexClusteringSqDistance_ )
		{
			if ( distance2 > vertexClusteringSqDistance_ ) counter++;
			oldX = p.x();
			oldY = p.y();
			oldZ = p.z();
		}
	}
	
	return counter+1;
}

std::pair<double, double> TrackCategorizer::calcPull(reco::TrackBaseRef const & track, TrackingParticleRef const & tpr) const
{
	
	// Compute tracking particle parameters at point of closest approach to the beamline
	
	const SimTrack * assocTrack = &(*tpr->g4Track_begin());
	
	FreeTrajectoryState ftsAtProduction(
		GlobalPoint(
			tpr->vertex().x(),
					tpr->vertex().y(),
					tpr->vertex().z()
		),
		GlobalVector(
			assocTrack->momentum().x(),
					 assocTrack->momentum().y(),
					 assocTrack->momentum().z()
		),
		TrackCharge(track->charge()),
										magneticField_.product()
	);
	
	try
	{
		TSCPBuilderNoMaterial tscpBuilder;
		TrajectoryStateClosestToPoint tsAtClosestApproach = tscpBuilder(
			ftsAtProduction,
			GlobalPoint(beamSpot_->x0(), beamSpot_->y0(), beamSpot_->z0())
		);
		
		GlobalVector v = tsAtClosestApproach.theState().position()
		- GlobalPoint(beamSpot_->x0(), beamSpot_->y0(), beamSpot_->z0());
		GlobalVector p = tsAtClosestApproach.theState().momentum();
		
		// Simulated dxy
		double dxySim = -v.x()*sin(p.phi()) + v.y()*cos(p.phi());
		
		// Simulated dz
		double dzSim = v.z() - (v.x()*p.x() + v.y()*p.y())*p.z()/p.perp2();
		
		// Calculate the dxy pull
		double dxyPull = std::abs(
			track->dxy( reco::TrackBase::Point(beamSpot_->x0(), beamSpot_->y0(), beamSpot_->z0()) ) - dxySim
		) / track->dxyError();
		
		// Calculate the dx pull
		double dzPull = std::abs(
			track->dz( reco::TrackBase::Point(beamSpot_->x0(), beamSpot_->y0(), beamSpot_->z0()) ) - dzSim
		) / track->dzError();
		
		// Return true if d0Pull > badD0Pull sigmas
		return std::make_pair(dxyPull, dzPull);
		
	}
	catch (cms::Exception exception)
	{
		return std::make_pair(-1, -1);
	}
}


TrackingParticleRef TrackCategorizer::getSourceTrack(TrackingVertexRef const & evaluatedVertex) const
{
	TrackingParticleRef output;
	
	TrackingVertex::tp_iterator itd, its;
	
	for (its = evaluatedVertex->sourceTracks_begin(); its != evaluatedVertex->sourceTracks_end(); ++its)
	{
		bool flag = false;
		for (itd = evaluatedVertex->daughterTracks_begin(); itd != evaluatedVertex->daughterTracks_end(); ++itd)
		{
			if (itd == its)
			{
				flag = true;
			}
		}
		if (!flag)
			break;
	}
	
	// Collect the pdgid of the original source track
	if ( its != evaluatedVertex->sourceTracks_end() )
		output = *its;
	
	return output;
}


int TrackCategorizer::getLongLivedGenParticle(TrackingVertexRef const & tv) const {
	
	HistoryBase::GenVertexTrailHelper genVertexTrailHelper;
	
	int pdgid = 0;
	
	for (TrackingVertex::genv_iterator iGV = tv->genVertices_begin(); iGV != tv->genVertices_end(); ++iGV){
		
		HepMC::GenVertex const * genVertex = &(**iGV);
		
		if ( genVertexTrailHelper.find(genVertex) != genVertexTrailHelper.end() )
			continue;
		
		genVertexTrailHelper.insert(genVertex);
		
		if ( genVertex->particles_in_size() )
		{
			double travelDistance; // in mm
			HepMC::GenVertex const * parentVertex = (*genVertex->particles_in_const_begin())->production_vertex();
			if (parentVertex)
				travelDistance = distanceToPrevGenVertex(genVertex, parentVertex);
			else
				travelDistance = 0;
			
			if (travelDistance > 0.01)	/// KEEP THIS VALUE? MAKE THIS AN INPUT TO THE ANALYZER?
			{
				pdgid = std::abs((*genVertex->particles_in_const_begin())->pdg_id());
				break;
			}
		}
	}
	
	return pdgid;
	
	
	//! only fill genVertexTrailHelper to check if GenVertex already existed
	//         genVertexTrail_.push_back(genVertex);
	// Verify if the vertex has incoming particles
	
}
