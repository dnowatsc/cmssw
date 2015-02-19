/*
 *  VertexCategorizer.C
 */

#include <math.h>
#include <cstdlib>
#include <iostream>

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "HepPDT/ParticleID.hh"

#include "SimTracker/VertexCategorization/interface/VertexCategorizer.h"

#include "SimTracker/Records/interface/TrackAssociatorRecord.h"


#include "DataFormats/Common/interface/PtrVector.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"

#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefToBaseVector.h"

#include "SimTracker/VertexCategorization/interface/TrackCategorizer.h"

int VertexCategorizer::vertexCount = 0;
bool VertexCategorizer::messageOutput = false;

// ostream& OutputController::operator<< (ostream& os)
// {
// 	if (printOutput_)
// 		return std::cout << os;
// 	else return std::cout;
// }



VertexCategorizer::VertexCategorizer(edm::Event const & event, edm::EventSetup const & setup, edm::ParameterSet const & config) :
        // quality_(config),
		HistoryBase(),
		trackProducer_( config.getUntrackedParameter<edm::InputTag>("trackProducer") ),
		vertexProducer_( config.getUntrackedParameter<edm::InputTag>("vertexProducer") )
        // beamSpotLabel_( config.getUntrackedParameter<edm::InputTag>("beamSpot") )
{
	reset();

	vertexCount = 0;

	event.getByLabel(trackProducer_, trackCollection_);

	event.getByLabel(vertexProducer_, vertexCollection_);

	for (size_t iVertex = 0; iVertex < vertexCollection_->size(); ++iVertex)
	{
		VertexMCInformation newVertex;
		edm::RefToBase<reco::VertexCompositePtrCandidate> vertRef = vertexCollection_->refAt(iVertex);

		newVertex.vertexRef = vertRef;

		reco::VertexCompositePtrCandidate const & vCand = (*vertexCollection_)[iVertex];


		// std::cout << "Vertex " << iVertex << ":" << std::endl;
		// std::cout << "  Position 1: " << vertRef->vx() << "|" << vertRef->vy() << "|" << vertRef->vz() << std::endl << std::endl;
		// std::cout << "  Track Positions 1:" << std::endl;

		for (size_t iDaughter = 0; iDaughter < vCand.numberOfSourceCandidatePtrs(); ++iDaughter)
		{
			const reco::Track * daughterTrack = vCand.sourceCandidatePtr(iDaughter)->bestTrack();
			for (size_t iTrack = 0; iTrack < trackCollection_->size(); ++iTrack)
			{
				edm::RefToBase<reco::Track> trackPtr = trackCollection_->refAt(iTrack);
				if (&(*trackPtr) == daughterTrack)
				{
					newVertex.daughterTracks.push_back(trackPtr);
					// std::cout << "    Track " << iDaughter << ":" << std::endl;
					// std::cout << "      Reco mother position 1: " << vCand.sourceCandidatePtr(iDaughter)->vx() << "|" << vCand.sourceCandidatePtr(iDaughter)->vy() << "|" << vCand.sourceCandidatePtr(iDaughter)->vz() << std::endl;
					// std::cout << "      Reco mother position 2: " << vCand.daughterPtr(iDaughter)->vx() << "|" << vCand.daughterPtr(iDaughter)->vy() << "|" << vCand.daughterPtr(iDaughter)->vz() << std::endl;
					// std::cout << "      Reco mother position 3: " << trackPtr->vertex().x() << "|" << trackPtr->vertex().y() << "|" << trackPtr->vertex().z() << std::endl;
					// std::cout << "      Reco mother position 4: " << trackPtr->referencePoint().x() << "|" << trackPtr->referencePoint().y() << "|" << trackPtr->referencePoint().z() << std::endl;

				}
			}
			if ( !daughterTrack)
				std::cout << "Track " << iDaughter << ": " << daughterTrack->pt() << " | " << newVertex.daughterTracks[newVertex.daughterTracks.size()-1]->pt() << std::endl;
		}

		// std::cout << std::endl;

		analyzedVertices_.push_back(newVertex);
	}

}


void VertexCategorizer::newEvent()
{
}

void VertexCategorizer::evaluate(edm::ParameterSet const & config, VertexMCInformation & vertInfo, TrackCategorizer & trackCategorizer)
{
	messageOutput = false;

	// std::cout << "Vertex " << vertexCount++ << ":" << std::endl;
	// std::cout << "Position: " << vertInfo.vertexRef->vx() << "|" << vertInfo.vertexRef->vy() << "|" << vertInfo.vertexRef->vz() << std::endl << std::endl;

	trackCategorizer.newTrackCollection(vertInfo.daughterTracks);

	double vertRho = std::sqrt(vertInfo.vertexRef->vx()*vertInfo.vertexRef->vx() + vertInfo.vertexRef->vy()*vertInfo.vertexRef->vy());

	// if (vertRho > 2.) messageOutput = true;

	if (messageOutput)
	{
		std::cout << "  Vertex " << vertexCount++ << std::endl;
		// std::cout << "  Position: " << vertInfo.vertexRef->vx() << "|" << vertInfo.vertexRef->vy() << "|" << vertInfo.vertexRef->vz() << std::endl << std::endl;
		std::cout << "  Rho: " << vertRho << std::endl << std::endl;
		std::cout << std::ends;
	}


	trackCategorizer.evaluate();
	if (messageOutput)
		std::cout << std::endl;

	vertInfo.analyzedTracks = trackCategorizer.returnAnalyzedTracks();

	for (TrackMCInformation & trackInfo : vertInfo.analyzedTracks)
	{
		// calculate trackWeigth here if it is possible
		trackInfo.trackWeight = 1.;
	}

}

