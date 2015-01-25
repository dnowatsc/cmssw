/*
 *  TrackCategorizer.C
 */

#include <math.h>
#include <cstdlib>
#include <iostream>

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
	// trackProducer_ = config.getUntrackedParameter<edm::InputTag>("trackProducer");
    
	// Set the history depth after hadronization
	// tracer_.depth(-2);

    // Set the minimum decay length for detecting long decays
	// longLivedDecayLength_ = config.getUntrackedParameter<double>("longLivedDecayLength");

    // Set the distance for clustering vertices
	float vertexClusteringDistance = config.getUntrackedParameter<double>("vertexClusteringDistance");
	vertexClusteringSqDistance_ = vertexClusteringDistance * vertexClusteringDistance;

	
	// Set the number of innermost layers to check for bad hits
	// numberOfInnerLayers_ = config.getUntrackedParameter<unsigned int>("numberOfInnerLayers");

    // Get the new event information for the tracer
	// tracer_.newEvent(event, setup);
    // Get hepmc of the event
	
	setup.get<IdealMagneticFieldRecord>().get(magneticField_);

    // Get the partivle data table
    setup.getData(particleDataTable_);
	
	// get the beam spot
	event.getByLabel(beamSpotLabel_, beamSpot_);

	event.getByLabel(hepMCLabel_, mcInformation_);

	event.getByLabel(candidatesLabel_, candidates_);

	// Get the track associator
	setup.get<TrackAssociatorRecord>().get(trackAssociatorProducer_, trackAssociator_);

	genPrimaryVertices();


    // Create the list of primary vertices associated to the event
	// genPrimaryVertices(TVCollection);

	// create a RefVector from the TrackingParticle collection; needed for the qTABH to work with a RefToBaseVector<reco::Track>

	event.getByLabel(trackingTruth_, TPCollection_);

	for (size_t iTP = 0; iTP < TPCollection_->size(); ++iTP)
	{
		edm::Ref<TrackingParticleCollection> tpRef(TPCollection_.product(), iTP);
		tpRefVec_.push_back(tpRef);
	}


	
	// 	std::cout << "GenPrimVert size: " << genpvs_.size() << std::endl << std::endl;
	
	// 	PUVerticesFinder(TVCollection);
}


// void TrackCategorizer::newEvent()
// {
// }

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
		// newTrackMCInfo.tpRefWithQualityPairs = iMatch->val;
		analyzedTracks_.push_back(newTrackMCInfo);
	}
}


void TrackCategorizer::evaluate (TrackMCInformation & trackMcInfo)
{

    std::cout << "    Track " << trackCount++ << ": " << std::endl;
    std::cout << "      Reco mother position: " << trackMcInfo.recoTrackRef->vertex().x() << "|" << trackMcInfo.recoTrackRef->vertex().y() << "|" << trackMcInfo.recoTrackRef->vertex().z() << std::endl;

    trackMcInfo.numberMatches = trackRecoToSim_[trackMcInfo.recoTrackRef].size();

    std::pair<TrackingParticleRef, double> result =  match(trackMcInfo.recoTrackRef, trackRecoToSim_, bestMatchByMaxValue_);
	
	TrackingParticleRef tpr( result.first );

	std::cout << "      True mother position: " << tpr->parentVertex()->position().x() << "|" << tpr->parentVertex()->position().y() << "|" << tpr->parentVertex()->position().z() << std::endl;

	trackMcInfo.matchingQuality = result.second;

	if (!tpr.isNull()){
		// 	std::cout << tpr->parentVertex()->position().x() << "/" << tpr->parentVertex()->position().y() << "/" << tpr->parentVertex()->position().z();

		HistoryBase::evaluate(tpr);
		trackMcInfo.vertexType = vertexInformationDist(this->genParticleTrail(), this->simParticleTrail());


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

			
		// 	if (vertexKindDist == 1){
		// 		flags_[PrimaryVertex] += recoDaughterWeight;
		// 	}
		// 	else {
		// 		if (tpr->trackPSimHit().empty()){
		// 			flags_[UnknownProcess] += recoDaughterWeight;
		// 			continue;
		// 		}

		// 		unsigned short process = tpr->pSimHit_begin()->processType();

		// 		std::cout << " | " << process;

		// 		if (process == G4::Hadronic) flags_[HadronicProcess] += recoDaughterWeight;
		// 		if (process == G4::Unknown) flags_[UnknownProcess] += recoDaughterWeight;
		// 		if (process == G4::Undefined) flags_[UndefinedProcess] += recoDaughterWeight;
		// 		if (process == G4::Compton) flags_[ComptonProcess] += recoDaughterWeight;
		// 		if (process == G4::Annihilation) flags_[AnnihilationProcess] += recoDaughterWeight;
		// 		if (process == G4::EIoni) flags_[EIoniProcess] += recoDaughterWeight;
		// 		if (process == G4::HIoni) flags_[HIoniProcess] += recoDaughterWeight;
		// 		if (process == G4::MuIoni) flags_[MuIoniProcess] += recoDaughterWeight;
		// 		if (process == G4::Photon) flags_[PhotonProcess] += recoDaughterWeight;
		// 		if (process == G4::MuPairProd) flags_[MuPairProdProcess] += recoDaughterWeight;
		// 		if (process == G4::EBrem) flags_[EBremProcess] += recoDaughterWeight;
		// 		if (process == G4::SynchrotronRadiation) flags_[SynchrotronRadiationProcess] += recoDaughterWeight;
		// 		if (process == G4::MuBrem) flags_[MuBremProcess] += recoDaughterWeight;
		// 		if (process == G4::Conversions) flags_[ConversionsProcess] += recoDaughterWeight;
		// 		if (process == G4::MuNucl) flags_[MuNuclProcess] += recoDaughterWeight;

		// 		if (process == G4::Primary || process == G4::Decay){
		// 			int pdgid = 0;

		// 			if (tpr->parentVertex()->nSourceTracks()){

		// 				TrackingParticleRef const & selectedTrack = getSourceTrack(tpr->parentVertex());

		// 				if (!selectedTrack.isNull())
		// 					pdgid = selectedTrack->pdgId();

		// 			} else if (tpr->parentVertex()->nGenVertices()){
		// 				pdgid = getLongLivedGenParticle(tpr->parentVertex());
		// 			}

		// 			std::cout << " | " << pdgid;

		// 			if (!pdgid){
		// 				flags_[UnknownDecay] += recoDaughterWeight;
		// 				continue;
		// 			}

		// 			HepPDT::ParticleID particleID(std::abs(pdgid));

		// 				// Check if the particle type is valid one
		// 			if (particleID.isValid())
		// 			{
		// 					// Check for B and C weak decays
		// 				if (particleID.hasBottom()) flags_[BWeakDecay] += recoDaughterWeight;
		// 				else if (checkBInHistory(tracer_.genParticleTrail()) || checkBInHistory(tracer_.simParticleTrail())){
		// 					if (particleID.hasCharm()) flags_[CFromBDecay] += recoDaughterWeight;
		// 					else flags_[OtherFromBDecay] += recoDaughterWeight;
		// 				}
		// 				else {
		// 					if (particleID.hasCharm()) flags_[CWeakDecay] += recoDaughterWeight;
		// 					if (std::abs(pdgid) == 211) flags_[ChargePionDecay] += recoDaughterWeight;
		// 					if (std::abs(pdgid) == 321) flags_[ChargeKaonDecay] += recoDaughterWeight;
		// 					if (std::abs(pdgid) == 15) flags_[TauDecay] += recoDaughterWeight;
		// 					if (std::abs(pdgid) == 310) flags_[KsDecay] += recoDaughterWeight;
		// 					if (std::abs(pdgid) == 3122) flags_[LambdaDecay] += recoDaughterWeight;
		// 					if (std::abs(pdgid) == 443) flags_[JpsiDecay] += recoDaughterWeight;
		// 					if (std::abs(pdgid) == 3312) flags_[XiDecay] += recoDaughterWeight;
		// 					if (std::abs(pdgid) == 3334) flags_[OmegaDecay] += recoDaughterWeight;
		// 					if (std::abs(pdgid) == 3222) flags_[SigmaPlusDecay] += recoDaughterWeight;
		// 					if (std::abs(pdgid) == 3112) flags_[SigmaMinusDecay] += recoDaughterWeight;
		// 				}


		// 				// Check for B or C pure leptonic decays
		// 				// int daughtId = abs((*iparticle)->pdgId());
		// 				// update(flags_[FromBWeakDecayMuon], particleID.hasBottom() && daughtId == 13);
		// 				// update(flags_[FromCWeakDecayMuon], particleID.hasCharm() && daughtId == 13);

		// 			}
		// 		}
		// 	}
				
		// 		/// DON'T CHECK QUALITY FOR NOW, PRODUCES SEGMENTATION VIOLATION PROBABLY IN TrackerHitAssociator::associateHitId() IN quality_.evaluate()
		// 		// qualityInformation(tracer_.simParticleTrail(), *iVertDaughter);
				
		}
		else {
		}
	
	
	// if (recoDaughterTotalWeight){
	// 	for (unsigned int i = 0; i < flags_.size(); ++i)
	// 		flags_[i] = flags_[i]/recoDaughterTotalWeight;
	// 	BadMatchingWeight_ = BadMatchingWeight_/recoDaughterTotalWeight;
	// 	BadTrackWeight_ = BadTrackWeight_/recoDaughterTotalWeight;
	// }
	
	// std::cout << std::endl << std::endl << " ";
	// for (unsigned int i = 0; i < flags_.size(); ++i)
	// 	std::cout << Names[i] << ":\t" << flags_[i] << std::endl;
	// std::cout << std::endl << " BadMatchingWeight: " << BadMatchingWeight_ << std::endl;
	// std::cout << " BadTrackWeight: " << BadTrackWeight_ << std::endl;
	// std::cout << std::endl;
			
 //    return *this;
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
			
			/// Look for close-by TVs to check their Event/BunchCrossing number
// 			TrackingVertexCollection::const_iterator iTV = TVCollection->begin();
// 			int TVindex = 0;
// 			
// 			for (; iTV != TVCollection->end(); ++iTV, ++TVindex){
// 				double distance = sqrt( pow(pv.x - iTV->position().x(), 2) + pow(pv.y - iTV->position().y(), 2) + pow(pv.z - iTV->position().z(), 2) );
// 				if ( distance < vertexClusteringSqDistance_ )
// 					break;
// 			}
// 			
// 			if (iTV != TVCollection->end())
// 				std::cout << "TV close to Primary Vertex found! Event / BunchCrossing number / index: " << iTV->eventId().event() << " " << iTV->eventId().bunchCrossing() << " " << TVindex << std::endl;
// 			else std::cout << "No TV close to Primary Vertex found!" << std::endl;

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


/// MOD: ignore simToReco for now

// TrackCategorizer const & TrackCategorizer::evaluate (TrackingVertexRef const & vertex)
// {
//     // Initializing the category vector
//     reset();
// 
//     // Trace the history for the given TP
//     tracer_.evaluate(vertex);
// 
//     // Check for a reconstructed track
//     if ( tracer_.recoVertex().isNonnull() )
//         flags_[Reconstructed] = true;
//     else
//         flags_[Reconstructed] = false;
// 
//     // Get all the information related to the simulation details
//     simulationInformation();
// 
//     // Get all the information related to decay process
//     processesAtGenerator();
// 
//     // Get information about conversion and other interactions
//     processesAtSimulation();
// 
//     // Get geometrical information about the vertices
//     vertexInformation();
// 
//     // Check for unkown classification
//     unknownVertex();
// 
//     return *this;
// }


// void TrackCategorizer::simulationInformation()
// {
//     // Get the event id for the initial TP.
//     EncodedEventId eventId = tracer_.simVertex()->eventId();
//     // Check for signal events
//     flags_[SignalEvent] = !eventId.bunchCrossing() && !eventId.event();
// }


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

// // const double TrackCategorizer::weight(G4::Process processQuery) const
// // {
// //     const reco::VertexBaseRef & recoVertex = tracer_.recoVertex();
// // 
// //     const reco::RecoToSimCollection & trackRecoToSim = tracer_.trackRecoToSim();
// // 
// //     if (!recoVertex.isNonnull()) {return -1;}
// // 
// //     int recoDaughterCounter = 0;
// //     double recoDaughterWeight = 0.;
// //     double processWeight = 0.;
// // 
// //     for (reco::Vertex::trackRef_iterator iVertDaughter = recoVertex->tracks_begin(); iVertDaughter != recoVertex->tracks_end(); ++iVertDaughter)
// //     {
// //         recoDaughterCounter++;
// // 
// //         // TRACK QUALITY, WHERE TO GET FROM?? in original vertexAssociator is obtained from config ParameterSet which again is coming from the EventSetup upon initialisation of the vertexAssociator; maybe initialize vertexAssociator and then get trackQuality_ from there? but is private member...
// //         // UPDATE: don't need because tracks have already been selected!!
// //         //       if ( !(*iVertDaughter)->quality(trackQuality_) ) continue;
// // 
// //         recoDaughterWeight += recoVertex->trackWeight(*iVertDaughter);
// // 
// //         /// Check for association for the given RecoDaughter
// //         if ( trackRecoToSim.numberOfAssociations(*iVertDaughter) > 0 )
// //         {
// //             const std::vector<std::pair<TrackingParticleRef,double> > & associations = trackRecoToSim[*iVertDaughter];
// // 
// //             std::map<double, TrackingParticleRef> associationsSorted;
// //             for (unsigned int i=0; i < associations.size(); ++i)
// //             {
// //                 associationsSorted[associations[i].second] = associations[i].first;
// //             }
// // 
// //             // Get a reference to parent vertex of TrackingParticle associated to the RecoDaughter
// //             // TrackingVertexRef trackingVertex = associationsSorted.rbegin()->second->parentVertex();
// //             // // Store matched RecoDaughter to the trackingVertex
// //             // matches_[trackingVertex].first += recoVertex.trackWeight(*iVertDaughter);
// //             // matches_[trackingVertex].second++;
// // 
// //             /// take only the first association with the highest quality
// //             unsigned short process = 0;
// // 
// //             //! needs to be changed to associationsSorted.rbegin()->second->parentVertex()->pSimVertex_begin()->processType() in CMSSW_7!
// // 
// //             if(!associationsSorted.rbegin()->second->trackPSimHit().empty())
// //             {
// //                 process = associationsSorted.rbegin()->second->pSimHit_begin()->processType();
// //             }
// //             else
// //             {}
// // 
// //             //! the entire following code needs to be rewritten in 7X as the process type numbering scheme has changed!
// //             if (process == processQuery)
// //             {
// //                 processWeight += recoVertex->trackWeight(*iVertDaughter);
// //             }
// // 
// //             //! ==========================================================================
// //             //! add more features here to also be able to ask for other categories, i.e. isBWeak etc.
// //             //! ==========================================================================
// // 
// //             //     if (process == G4::Hadronic) {Hadronic += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::Unknown) {Unknown += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::Undefined) {Undefined += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::Decay) {Decay += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::Primary) {GeantPrimary += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::Compton) {Compton += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::Annihilation) {Annihilation += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::EIoni) {EIoni += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::HIoni) {HIoni += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::MuIoni) {MuIoni += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::Photon) {Photon += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::MuPairProd) {MuPairProd += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::EBrem) {EBrem += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::SynchrotronRadiation) {SynchrotronRadiation += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::MuBrem) {MuBrem += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::Conversions) {Conversions += recoVertex.trackWeight(*iVertDaughter);}
// //             // if (process == G4::MuNucl) {MuNucl += recoVertex.trackWeight(*iVertDaughter);}
// //         }
// // 
// //             //! TODO: include "NoProcess" weight in case 'if(!associationsSorted.rbegin()->second->trackPSimHit().empty())' fails or "NoAssociation" weight in case 'if ( trackRecoToSim_.numberOfAssociations(*iVertDaughter) > 0 )' fails or make a combined weight of these two
// // 
// //     }
// // 
// //     return processWeight;
// // }


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

// void TrackCategorizer::qualityInformation(HistoryBase::SimParticleTrail const & spt, reco::TrackBaseRef const & track)
// {
// 	// run the hit-by-hit reconstruction quality analysis
// 	std::cout << "TEST1" << std::endl;
// 	quality_.evaluate(spt, track);
// 	std::cout << "TEST2" << std::endl;
	
// 	unsigned int maxLayers = std::min(numberOfInnerLayers_, quality_.numberOfLayers());
	
// 	// check the innermost layers for bad hits
// 	for (unsigned int i = 0; i < maxLayers; i++)
// 	{
// 		const TrackQuality::Layer &layer = quality_.layer(i);
		
// 		// check all hits in that layer
// 		for (unsigned int j = 0; j < layer.hits.size(); j++)
// 		{
// 			const TrackQuality::Layer::Hit &hit = layer.hits[j];
			
// 			// In those cases the bad hit was used by track reconstruction
// 			if (hit.state == TrackQuality::Layer::Noise ||
// 				hit.state == TrackQuality::Layer::Misassoc)
// 				BadInnerHits_ = true;
// 			else if (hit.state == TrackQuality::Layer::Shared)
// 				SharedInnerHits_ = true;
// 		}
// 	}
// }

// /* METHOD BELOW FROM VERTEX CLASSIFIER! Works somehow different (using vertex clusters, I don't really understand it) but apparently
//  * yields the same result, i.e. whether it is primary/secondary/tertiary vertex...
//  * => use vertexInformationDist!

// int TrackCategorizer::vertexInformationClust()
// {
//     // Helper class for clusterization
//     typedef std::multimap<double, HepMC::ThreeVector> Clusters;
//     typedef std::pair<double, HepMC::ThreeVector> ClusterPair;

//     Clusters clusters;

//     // Get the main primary vertex from the list
//     GeneratedPrimaryVertex const & genpv = genpvs_.back();

//     // Get the generated history of the tracks
//     HistoryBase::GenVertexTrail & genVertexTrail = const_cast<HistoryBase::GenVertexTrail &> (tracer_.genVertexTrail());

//     // Unit transformation from mm to cm
//     double const mm = 0.1;

//     // Loop over the generated vertexes
//     for (
//         HistoryBase::GenVertexTrail::const_iterator ivertex = genVertexTrail.begin();
//         ivertex != genVertexTrail.end();
//         ++ivertex
//     )
//     {
//         // Check vertex exist
//         if (*ivertex)
//         {
//             // Measure the distance2 respecto the primary vertex
//             HepMC::ThreeVector p = (*ivertex)->point3d();
//             double distance = sqrt( pow(p.x() * mm - genpv.x, 2) + pow(p.y() * mm - genpv.y, 2) + pow(p.z() * mm - genpv.z, 2) );

//             // If there is not any clusters add the first vertex.
//             if ( clusters.empty() )
//             {
//                 clusters.insert( ClusterPair(distance, HepMC::ThreeVector(p.x() * mm, p.y() * mm, p.z() * mm)) );
//                 continue;
//             }

//             // Check if there is already a cluster in the given distance from primary vertex
//             Clusters::const_iterator icluster = clusters.lower_bound(distance - vertexClusteringSqDistance_);

//             if ( icluster == clusters.upper_bound(distance + vertexClusteringSqDistance_) )
//             {
//                 clusters.insert ( ClusterPair(distance, HepMC::ThreeVector(p.x() * mm, p.y() * mm, p.z() * mm)) );
//                 continue;
//             }

//             bool cluster = false;

//             // Looping over the vertex clusters of a given distance from primary vertex
//             for (;
//                     icluster != clusters.upper_bound(distance + vertexClusteringSqDistance_);
//                     ++icluster
//                 )
//             {
//                 double difference = sqrt (
//                                         pow(p.x() * mm - icluster->second.x(), 2) +
//                                         pow(p.y() * mm - icluster->second.y(), 2) +
//                                         pow(p.z() * mm - icluster->second.z(), 2)
//                                     );

//                 if ( difference < vertexClusteringSqDistance_ )
//                 {
//                     cluster = true;
//                     break;
//                 }
//             }

//             if (!cluster) clusters.insert ( ClusterPair(distance, HepMC::ThreeVector(p.x() * mm, p.y() * mm, p.z() * mm)) );
//         }
//     }

//     HistoryBase::SimVertexTrail & simVertexTrail = const_cast<HistoryBase::SimVertexTrail &> (tracer_.simVertexTrail());

//     // Loop over the generated particles
//     for (
//         HistoryBase::SimVertexTrail::const_reverse_iterator ivertex = simVertexTrail.rbegin();
//         ivertex != simVertexTrail.rend();
//         ++ivertex
//     )
//     {
//         // Look for those with production vertex
//         TrackingVertex::LorentzVector p = (*ivertex)->position();

//         double distance = sqrt( pow(p.x() - genpv.x, 2) + pow(p.y() - genpv.y, 2) + pow(p.z() - genpv.z, 2) );

//         // If there is not any clusters add the first vertex.
//         if ( clusters.empty() )
//         {
//             clusters.insert( ClusterPair(distance, HepMC::ThreeVector(p.x(), p.y(), p.z())) );
//             continue;
//         }

//         // Check if there is already a cluster in the given distance from primary vertex
//         Clusters::const_iterator icluster = clusters.lower_bound(distance - vertexClusteringSqDistance_);

//         if ( icluster == clusters.upper_bound(distance + vertexClusteringSqDistance_) )
//         {
//             clusters.insert ( ClusterPair(distance, HepMC::ThreeVector(p.x(), p.y(), p.z())) );
//             continue;
//         }

//         bool cluster = false;

//         // Looping over the vertex clusters of a given distance from primary vertex
//         for (;
//                 icluster != clusters.upper_bound(distance + vertexClusteringSqDistance_);
//                 ++icluster
//             )
//         {
//             double difference = sqrt (
//                                     pow(p.x() - icluster->second.x(), 2) +
//                                     pow(p.y() - icluster->second.y(), 2) +
//                                     pow(p.z() - icluster->second.z(), 2)
//                                 );

//             if ( difference < vertexClusteringSqDistance_ )
//             {
//                 cluster = true;
//                 break;
//             }
//         }

//         if (!cluster) clusters.insert ( ClusterPair(distance, HepMC::ThreeVector(p.x(), p.y(), p.z())) );
//     }

//     if ( clusters.size() == 1 )
//         return 1;
//     else if ( clusters.size() == 2 )
//         return 2;
//     else
//         return 3;
// }

// //*/


// bool TrackCategorizer::isFinalstateParticle(const HepMC::GenParticle * p)
// {
//     return !p->end_vertex() && p->status() == 1;
// }


// bool TrackCategorizer::isCharged(const HepMC::GenParticle * p)
// {
//     const ParticleData * part = particleDataTable_->particle( p->pdg_id() );
//     if (part)
//         return part->charge()!=0;
//     else
//     {
//         // the new/improved particle table doesn't know anti-particles
//         return  particleDataTable_->particle( -p->pdg_id() ) != 0;
//     }
// }


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
