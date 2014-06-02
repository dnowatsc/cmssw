#include <algorithm>

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "SimTracker/VertexHistory/interface/HistoryBaseOwn.h"

#include "HepPDT/ParticleID.hh"


bool HistoryBaseOwn::traceGenHistory(HepMC::GenParticle const * genParticle, Flags & flags, int depth, double weight)
{
    if (debugMsg_)
      std::cout << "    Check out next GenParticle, depth " << depth << std::endl;
    if (debugMsg_)
    {
      std::cout << "       Status and PdgId: " << genParticle->status() << " " << genParticle->pdg_id() << std::endl << std::endl;
      if (genParticle->production_vertex())
      {
	if (genParticle->production_vertex()->particles_in_size())
	  std::cout << "       PdgId of of this particle's parent: " << (*genParticle->production_vertex()->particles_in_const_begin())->pdg_id() << std::endl;
	else
	  std::cout << "       No parent found for GenParticle!" << std::endl;
      }
    }
    // Third stop criteria: status abs(depth_) particles after the hadronization.
    // The after hadronization is done by detecting the pdg_id pythia code from 88 to 99
    if ( genParticle->status() <= std::abs(status_) && (genParticle->pdg_id() < 88 || genParticle->pdg_id() > 99) )
    {
//         genParticleTrail_.push_back(genParticle);
        // Get the producer vertex and trace it history
        return traceGenHistory( genParticle->production_vertex(), flags, depth, weight);
    }
    else
    {
      if (debugMsg_)
	  std::cout << "    GenParticle has wrong status or wrong pdgid!" << std::endl << std::endl;
      if (genParticle->status() == 3)
      {
	  if (debugMsg_)
	      std::cout << "    Primary Process detected!" << std::endl << std::endl;
	  flagProcess(flags, 1, weight, false);
	  return false;
      }
      
      return true;
    }
}


bool HistoryBaseOwn::traceGenHistory(HepMC::GenVertex const * genVertex, Flags & flags, int depth, double weight)
{
        
    // Verify if has a vertex associated
    if (genVertex)
    {
        if (debugMsg_)
	{
	  std::cout << "    Check out next GenVertex, depth " << depth << std::endl;
	  std::cout << "    Dump Vertex information:" << std::endl;
	  genVertex->print();
	}
        // Skip if already exist in the collection
        if ( genVertexTrailHelper_.find(genVertex) != genVertexTrailHelper_.end() )
	{
	    if (debugMsg_)
	      std::cout << "    GenVertex already exists." << std::endl << std::endl;
            return true;
	}
        //! only fill genVertexTrailHelper to check if GenVertex already existed
//         genVertexTrail_.push_back(genVertex);
        genVertexTrailHelper_.insert(genVertex);
        // Verify if the vertex has incoming particles
        if ( genVertex->particles_in_size() )
	{
	    double travelDistance; // in mm
	    HepMC::GenVertex const * parentVertex = (*genVertex->particles_in_const_begin())->production_vertex();
	    if (parentVertex)
		travelDistance = distanceToPrevVertex(genVertex, parentVertex);
	    else
		travelDistance = 0;
	    
	    if (debugMsg_)
		std::cout << "    Travel distance to parent GenVertex: " << travelDistance << std::endl << std::endl;
	    
	    if (travelDistance > 0.01)
	    {
		int pdgid = std::abs((*genVertex->particles_in_const_begin())->pdg_id());
		if (debugMsg_)
		    std::cout << "    Long-lived particle: " << pdgid << std::endl;
		
		if (std::abs(depth) >= histSteps_)
		{
		    if (debugMsg_)
			std::cout << "    Flagged!" << pdgid << std::endl;
		    flagProcess(flags, pdgid, weight);
		}
		
		if (depth == histSteps_)
		{
		    if (debugMsg_)
			std::cout << "    Max depth reached!" << std::endl << std::endl;
		    return true;
		}
		
		return traceGenHistory( *(genVertex->particles_in_const_begin()), flags, depth-1, weight);
	    }
	    else
	    {
		return traceGenHistory(*(genVertex->particles_in_const_begin()), flags, depth, weight);
	    }
	    
	}
	else
	{
	  if (debugMsg_)
	    std::cout << "    GenVertex has no incoming Particles." << std::endl << std::endl;
	  return true;
	}
    }
    return true;
}


bool HistoryBaseOwn::traceSimHistory(TrackingParticleRef const & trackingParticle, Flags & flags, int depth, double weight)
{
    // first stop condition: if the required depth is reached
//     if ( depth == depth_ && depth_ >= 0 ) return true;
    
    if (debugMsg_)
      std::cout << "    Check out Tracking Particle, depth " << depth << std::endl;

    // sencond stop condition: if a gen particle is associated to the TP
    if ( !trackingParticle->genParticle().empty() )
    {
        if (debugMsg_)
	  std::cout << "    Tracking Particle " << trackingParticle->pdgId() << " has GenParticle!" << std::endl;
        return traceGenHistory(&(**(trackingParticle->genParticle_begin())), flags, depth, weight);
    }

    if (debugMsg_)
      std::cout << "    No GenParticle image for " << trackingParticle->pdgId() << ", going on to parent Vertex." << std::endl;

    // get a reference to the TP's parent vertex and trace it history
    if (traceSimHistory(trackingParticle->parentVertex(), flags, depth, weight))
	return true;
    return true;
}


bool HistoryBaseOwn::traceSimHistory(TrackingVertexRef const & trackingVertex, Flags & flags, int depth, double weight)
{
    // verify if the parent vertex exists
    if ( trackingVertex.isNonnull() )
    {
        if (debugMsg_)
	{
	  std::cout << "  Check out Tracking Vertex, depth " << depth << std::endl;
	  std::cout << "    Position: " << trackingVertex->position().x() << " " << trackingVertex->position().y() << " " <<  trackingVertex->position().z() << " " << std::endl;
	}
	
        //! don't use trails any longer, flag directly!
//         simVertexTrail_.push_back(trackingVertex);
	if ( !trackingVertex->genVertices().empty() )
        {
            
            bool deactivateDebugMsg = false;
            if (debugMsg_)
	    {
	      std::cout << "    Vertex " << depth << " has " << trackingVertex->genVertices().size() << " GenVertices!" << std::endl;
	      if (trackingVertex->genVertices().size() > 50)
	      {
		std::cout << "    PU vertex, deactivate control output!" << std::endl << std::endl;
		deactivateDebugMsg = true;
		debugMsg(false);
	      }
	    }
	    
	    /// navigate over all the associated generated vertexes
            for ( TrackingVertex::genv_iterator ivertex = trackingVertex->genVertices_begin(); ivertex != trackingVertex->genVertices_end(); ++ivertex )
	    {
	      if (debugMsg_)
		  std::cout << "    GENVERTEX No. " << ivertex-trackingVertex->genVertices_begin() << std::endl;
	      if (!traceGenHistory(&(**(ivertex)), flags, depth, weight))
		  break;
	    }
	    if (deactivateDebugMsg)
	      debugMsg(true);
            return true;
        }
        else if ( !trackingVertex->sourceTracks().empty() )
        {

            // select the original source in case of combined vertices
            TrackingParticleRef const & sourceTrack = getSourceTrack(trackingVertex);
	    if (sourceTrack.isNull())
	    {
		if (debugMsg_)
		    std::cout <<  "    WARNING: Only looping source tracks found." << std::endl;
		return false;
	    }

            // verify if the new particle is not in the trail (looping partiles)
            if ( std::find( simParticleTrail_.begin(), simParticleTrail_.end(), sourceTrack) != simParticleTrail_.end() )
            {
	        if (debugMsg_)
		  std::cout <<  "    WARNING: Looping track found." << std::endl;
                return false;
            }

            // save particle in the trail
            if (debugMsg_) {
	      std::cout << "      Tracking Particle " << depth << " PdgId: " << sourceTrack->pdgId() << std::endl;
	    }
	    
	    //! use this trail only for checking if particle was already checked
            simParticleTrail_.push_back(sourceTrack);
	    
	    if (std::abs(depth) >= histSteps_)
	    {
		if (debugMsg_)
		    std::cout << "    Flagged!" << std::endl << std::endl;
		flagProcess(flags, std::abs(sourceTrack->pdgId()), weight);
	    }
	    if (depth == histSteps_)
	    {
		if (debugMsg_)
		    std::cout << "    Max depth reached!" << std::endl << std::endl;
		return true;
	    }
            return traceSimHistory (sourceTrack, flags, depth-1, weight);
        }
        else
        {
	    if (debugMsg_)
	      std::cout <<  "    WARNING: Tracking Vertex has neither source tracks nor GenVertices." << std::endl;
        }
    }
    else
    {
        if (debugMsg_)
	  std::cout << "    WARNING: Vertex cannot be found.";
    }

    return false;
}



void HistoryBaseOwn::flagProcess(Flags & flags_, int pdgid, double flagValue, bool cumulative)
{
  // Get particle type
  HepPDT::ParticleID particleID(pdgid);
  
  // Check if the particle type is valid one
  if (particleID.isValid())
  {
    // Get particle data
    ParticleData const * particleData = particleDataTable_->particle(particleID);
    // Check if the particle exist in the table
    if (!cumulative)
    {
      if (particleData)
      {
	flags_[BWeakDecay] = (!flags_[BWeakDecay] && particleID.hasBottom()) ? flagValue : flags_[BWeakDecay];
	flags_[CWeakDecay] = (!flags_[CWeakDecay] && particleID.hasCharm()) ? flagValue : flags_[CWeakDecay];
	// Check for B and C pure leptonic decay => NEED THIS?
	//       int daughterId = abs((*iparticle)->pdg_id());
	//       flags_[FromBWeakDecayMuon] = (!flags_[FromBWeakDecayMuon] && particleID.hasBottom() && daughterId == 13) ? flagValue : flags_[FromBWeakDecayMuon];
	//       flags_[FromCWeakDecayMuon] = (!flags_[FromCWeakDecayMuon] && particleID.hasCharm() && daughterId == 13) ? flagValue : flags_[FromCWeakDecayMuon];	
      }
      // Check Tau, Ks and Lambda decay
      flags_[PrimaryProcess] = (!flags_[PrimaryProcess] && pdgid == 1) ? flagValue : flags_[PrimaryProcess];
      flags_[ProtonDecay] = (!flags_[ProtonDecay] && pdgid == 2212) ? flagValue : flags_[ProtonDecay];
      flags_[ChargePionDecay] = (!flags_[ChargePionDecay] && pdgid == 211) ? flagValue : flags_[ChargePionDecay];
      flags_[ChargeKaonDecay] = (!flags_[ChargeKaonDecay] && pdgid == 321) ? flagValue : flags_[ChargeKaonDecay];
      flags_[TauDecay] = (!flags_[TauDecay] && pdgid == 15) ? flagValue : flags_[TauDecay];
      flags_[KsDecay] = (!flags_[KsDecay] && pdgid == 310) ? flagValue : flags_[KsDecay];
      flags_[LambdaDecay] = (!flags_[LambdaDecay] && pdgid == 3122) ? flagValue : flags_[LambdaDecay];
      flags_[JpsiDecay] = (!flags_[JpsiDecay] && pdgid == 443) ? flagValue : flags_[JpsiDecay];
      flags_[XiDecay] = (!flags_[XiDecay] && pdgid == 3312) ? flagValue : flags_[XiDecay];
      flags_[SigmaPlusDecay] = (!flags_[SigmaPlusDecay] && pdgid == 3222) ? flagValue : flags_[SigmaPlusDecay];
      flags_[SigmaMinusDecay] = (!flags_[SigmaMinusDecay] && pdgid == 3112) ? flagValue : flags_[SigmaMinusDecay];
    }
    else
    {
      if (particleData)
      {
	flags_[BWeakDecay] = (particleID.hasBottom()) ? flagValue+flags_[BWeakDecay] : flags_[BWeakDecay];
	flags_[CWeakDecay] = (particleID.hasCharm()) ? flagValue+flags_[CWeakDecay] : flags_[CWeakDecay];
	// Check for B and C pure leptonic decay => NEED THIS?
	//       int daughterId = abs((*iparticle)->pdg_id());
	//       flags_[FromBWeakDecayMuon] = (!flags_[FromBWeakDecayMuon] && particleID.hasBottom() && daughterId == 13) ? flagValue : flags_[FromBWeakDecayMuon];
	//       flags_[FromCWeakDecayMuon] = (!flags_[FromCWeakDecayMuon] && particleID.hasCharm() && daughterId == 13) ? flagValue : flags_[FromCWeakDecayMuon];	
      }
      // Check Tau, Ks and Lambda decay
      flags_[PrimaryProcess] = (pdgid == 1) ? flagValue+flags_[PrimaryProcess] : flags_[PrimaryProcess];
      flags_[ProtonDecay] = (pdgid == 2212) ? flagValue+flags_[ProtonDecay] : flags_[ProtonDecay];
      flags_[ChargePionDecay] = (pdgid == 211) ? flagValue+flags_[ChargePionDecay] : flags_[ChargePionDecay];
      flags_[ChargeKaonDecay] = (pdgid == 321) ? flagValue+flags_[ChargeKaonDecay] : flags_[ChargeKaonDecay];
      flags_[TauDecay] = (pdgid == 15) ? flagValue+flags_[TauDecay] : flags_[TauDecay];
      flags_[KsDecay] = (pdgid == 310) ? flagValue+flags_[KsDecay] : flags_[KsDecay];
      flags_[LambdaDecay] = (pdgid == 3122) ? flagValue+flags_[LambdaDecay] : flags_[LambdaDecay];
      flags_[JpsiDecay] = (pdgid == 443) ? flagValue+flags_[JpsiDecay] : flags_[JpsiDecay];
      flags_[XiDecay] = (pdgid == 3312) ? flagValue+flags_[XiDecay] : flags_[XiDecay];
      flags_[SigmaPlusDecay] = (pdgid == 3222) ? flagValue+flags_[SigmaPlusDecay] : flags_[SigmaPlusDecay];
      flags_[SigmaMinusDecay] = (pdgid == 3112) ? flagValue+flags_[SigmaMinusDecay] : flags_[SigmaMinusDecay];
    }
  }
  
  return;
}

TrackingParticleRef HistoryBaseOwn::getSourceTrack(TrackingVertexRef const & evaluatedVertex) const
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

double HistoryBaseOwn::distanceToPrevVertex (HepMC::GenVertex const * thisVertex, HepMC::GenVertex const * motherVertex) const
{
    HepMC::ThreeVector thisVertPos = thisVertex->point3d();
    HepMC::ThreeVector motherVertPos = motherVertex->point3d();
    HepMC::ThreeVector flightVector;
    flightVector.set(thisVertPos.x()-motherVertPos.x(), thisVertPos.y()-motherVertPos.y(), thisVertPos.z()-motherVertPos.z());
    double travelDistance = flightVector.r(); //! in mm
    
    return travelDistance;
}








