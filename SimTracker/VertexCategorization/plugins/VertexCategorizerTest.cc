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

#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"

#include "SimTracker/VertexCategorization/interface/VertexCategorizer.h"
#include "SimTracker/VertexCategorization/interface/TrackCategorizer.h"
#include "SimTracker/VertexCategorization/interface/CategoryClasses.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
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

    // VertexCategorizer vertexClassifier_;

    const edm::ParameterSet pSet;

    TH1F *geantProcessTypes, *cmsProcessTypes;

    // edm::ESHandle<ParticleDataTable> pdt_;

};


VertexCategorizerTest::VertexCategorizerTest(const edm::ParameterSet& config) :
    // vertexClassifier_(config),
    pSet(config)
{
    edm::Service<TFileService> fs;

    geantProcessTypes = fs->make<TH1F>("GeantTypes", "Geant Process Types", 300, -0.5, 300 - 0.5);
    cmsProcessTypes = fs->make<TH1F>("CMSTypes", "CMS Process Types", 300, -0.5, 300 - 0.5);

}


void VertexCategorizerTest::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
    // Set the classifier for a new event
    VertexCategorizer vertexCategorizer(event, setup, pSet);

    TrackCategorizer trackCategorizer(event, setup, pSet);
	
// 	std::cout << vertexCollection->size() << std::endl;
	
	// edm::RefToBaseVector<reco::Vertex> vertRefVec(vertexCollection);
	// for (unsigned int j=0; j<vertexCollection->size();j++)
	// 	vertRefVec.push_back(edm::RefToBase<reco::Vertex>(vertexCollection,j));
	
	// std::cout << "VertexCollection size: " << vertRefVec.size() << std::endl;
	
    vertexCategorizer.evaluate(pSet, trackCategorizer);

    for (const VertexMCInformation & vertInfo : vertexCategorizer.returnAnalyzedVertices())
    {
        for (const TrackMCInformation & trackInfo : vertInfo.analyzedTracks)
        {
            geantProcessTypes->Fill(trackInfo.geantProcessType);
            cmsProcessTypes->Fill(trackInfo.cmsProcessType);

        }
    }


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
