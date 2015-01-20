/*
 *  VertexCategorizerTest.C
 *
 *  Created by Victor Eduardo Bazterra on 5/31/07.
 *
 */

// system include files
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

// user include files
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
#include "SimTracker/VertexCategorization/interface/VertexCategorizer2.h"
 #include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"

//
// class decleration
//

class VertexCategorizerTest : public edm::EDAnalyzer
{
public:

    explicit VertexCategorizerTest(const edm::ParameterSet&);

private:

  virtual void beginRun(const edm::Run&,const edm::EventSetup&);
    virtual void beginJob() ;
    virtual void analyze(const edm::Event&, const edm::EventSetup&);

    // Member data

    VertexCategorizer2 vertexClassifier_;

    const edm::ParameterSet pSet;

    // edm::ESHandle<ParticleDataTable> pdt_;

};


VertexCategorizerTest::VertexCategorizerTest(const edm::ParameterSet& config) : vertexClassifier_(config), pSet(config)
{
}


void VertexCategorizerTest::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
    // Set the classifier for a new event
    vertexClassifier_.newEvent(event, setup);
	
// 	std::cout << vertexCollection->size() << std::endl;
	
	// edm::RefToBaseVector<reco::Vertex> vertRefVec(vertexCollection);
	// for (unsigned int j=0; j<vertexCollection->size();j++)
	// 	vertRefVec.push_back(edm::RefToBase<reco::Vertex>(vertexCollection,j));
	
	// std::cout << "VertexCollection size: " << vertRefVec.size() << std::endl;
	
    vertexClassifier_.evaluate(pSet);

	// for (edm::RefToBaseVector<reco::Vertex>::const_iterator iVertex = vertRefVec.begin(); iVertex < vertRefVec.end(); ++iVertex)
	// {
	// 	vertexClassifier_.evaluate(*iVertex);
	// }

    // Get a constant reference to the track history associated to the classifier
//     VertexHistory const & tracer = vertexClassifier_.history();

    // Loop over the track collection.
    
}


void
VertexCategorizerTest::beginRun(const edm::Run& run, const edm::EventSetup& setup)
{
    // Get the particles table.
    // setup.getData( pdt_ );
}

void
VertexCategorizerTest::beginJob()
{

}


DEFINE_FWK_MODULE(VertexCategorizerTest);
