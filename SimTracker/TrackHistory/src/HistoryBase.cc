#include <algorithm>

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "SimTracker/TrackHistory/interface/HistoryBase.h"


void HistoryBase::traceGenHistory(HepMC::GenParticle const * genParticle)
{
    if (debugMsg_)
      std::cout << "    Check out GenParticle." << std::endl;
    if (debugMsg_)
    {
      std::cout << "       Status and PdgId: " << genParticle->status() << " " << genParticle->pdg_id() << std::endl;
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
    if ( genParticle->status() <= abs(depth_) && (genParticle->pdg_id() < 88 || genParticle->pdg_id() > 99) )
    {
        genParticleTrail_.push_back(genParticle);
        // Get the producer vertex and trace it history
        traceGenHistory( genParticle->production_vertex() );
    }
    else
      if (debugMsg_)
	std::cout << "    GenParticle has wrong status or wrong pdgid!" << std::endl << std::endl;
}


void HistoryBase::traceGenHistory(HepMC::GenVertex const * genVertex)
{
    // Verify if has a vertex associated
    if (genVertex)
    {
        if (debugMsg_)
	{
	  std::cout << "    Check out Parent GenVertex." << std::endl;
	  std::cout << "    Dump Vertex information:" << std::endl;
	  genVertex->print();
	}
        // Skip if already exist in the collection
        if ( genVertexTrailHelper_.find(genVertex) != genVertexTrailHelper_.end() )	//! what is genVertexTrailHelper and what does it do??
	{
	    if (debugMsg_)
	      std::cout << "    GenVertex already exists." << std::endl << std::endl;
            return;
	}
        // Add vertex to the history
        genVertexTrail_.push_back(genVertex);
        genVertexTrailHelper_.insert(genVertex);
        // Verify if the vertex has incoming particles
        if ( genVertex->particles_in_size() )
            traceGenHistory( *(genVertex->particles_in_const_begin()) );
	else
	  if (debugMsg_)
	    std::cout << "    GenVertex has no incoming Particles." << std::endl << std::endl;
    }
}


bool HistoryBase::traceSimHistory(TrackingParticleRef const & trackingParticle, int depth)
{
    // first stop condition: if the required depth is reached
    if ( depth == depth_ && depth_ >= 0 ) return true;
    
    if (debugMsg_)
      std::cout << "    Check out Tracking Particle." << std::endl;

    // sencond stop condition: if a gen particle is associated to the TP
    if ( !trackingParticle->genParticle().empty() )
    {
        if (debugMsg_)
	  std::cout << "    Tracking Particle " << trackingParticle->pdgId() << " has GenParticle!" << std::endl;
        traceGenHistory(&(**(trackingParticle->genParticle_begin())));
        return true;
    }

    if (debugMsg_)
      std::cout << "    No GenParticle image for " << trackingParticle->pdgId() << ", going on to parent Vertex." << std::endl;

    // get a reference to the TP's parent vertex and trace it history
    return traceSimHistory( trackingParticle->parentVertex(), depth );
}


bool HistoryBase::traceSimHistory(TrackingVertexRef const & trackingVertex, int depth)
{
    // verify if the parent vertex exists
    if ( trackingVertex.isNonnull() )
    {
        if (debugMsg_)
	  std::cout << "    Check out Tracking Vertex." << std::endl;
        // save the vertex in the trail
        simVertexTrail_.push_back(trackingVertex);

        if ( !trackingVertex->sourceTracks().empty() )
        {

            // select the original source in case of combined vertices
            bool flag = false;
            TrackingVertex::tp_iterator itd, its;

            for (its = trackingVertex->sourceTracks_begin(); its != trackingVertex->sourceTracks_end(); its++)
            {
                for (itd = trackingVertex->daughterTracks_begin(); itd != trackingVertex->daughterTracks_end(); itd++)
                    if (itd != its)
                    {
                        flag = true;
                        break;
                    }
                if (flag)
                    break;
            }

            // verify if the new particle is not in the trail (looping partiles)
            if (
                std::find(
                    simParticleTrail_.begin(),
                    simParticleTrail_.end(),
                    *its
                ) != simParticleTrail_.end()
            )
            {
	        if (debugMsg_)
		  std::cout <<  "    WARNING: Looping track found." << std::endl;
                return false;
            }

            // save particle in the trail
            if (debugMsg_)
	      std::cout << "      " << (*its)->pdgId() << std::endl;
            simParticleTrail_.push_back(*its);
            return traceSimHistory (*its, --depth);
        }
        else if ( !trackingVertex->genVertices().empty() )
        {
            // navigate over all the associated generated vertexes
            bool deactivateDebugMsg = false;
            if (debugMsg_)
	    {
	      std::cout << "    Vertex has " << trackingVertex->genVertices().size() << " GenVertices!" << std::endl;
	      if (trackingVertex->genVertices().size() > 50)
	      {
		std::cout << "    PU vertex, deactivate control output!" << std::endl << std::endl;
		deactivateDebugMsg = true;
		debugMsg(false);
	      }
	    }
            for (
                TrackingVertex::genv_iterator ivertex = trackingVertex->genVertices_begin();
                ivertex != trackingVertex->genVertices_end();
                ++ivertex
            )
	    {
	      if (debugMsg_)
		std::cout << "    NEXT GENVERTEX" << std::endl;
	      traceGenHistory(&(**(ivertex)));
	    }
	    if (deactivateDebugMsg)
	      debugMsg(true);
            return true;
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

