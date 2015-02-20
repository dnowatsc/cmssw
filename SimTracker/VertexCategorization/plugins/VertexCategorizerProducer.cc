/*
 *  VertexCategorizerProducer.C
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


//
// class decleration
//

class VertexCategorizerProducer : public edm::EDProducer
{
public:

    explicit VertexCategorizerProducer(const edm::ParameterSet&);

private:

    virtual void beginRun(const edm::Run&,const edm::EventSetup&);
    virtual void beginJob() ;
    virtual void produce(edm::Event&, const edm::EventSetup&);

    // Member data

    // VertexCategorizer vertexClassifier_;

    const edm::ParameterSet pSet;

    // edm::ESHandle<ParticleDataTable> pdt_;

};


VertexCategorizerProducer::VertexCategorizerProducer(const edm::ParameterSet& config) :
    // vertexClassifier_(config),
    pSet(config)
{
    produces<std::vector<VertexMCInformation>>();
}


void VertexCategorizerProducer::produce(edm::Event& event, const edm::EventSetup& setup)
{
    // Set the classifier for a new event
    std::cout << "test0" << std::endl;
    VertexCategorizer vertexCategorizer(event, setup, pSet);

    std::cout << "test1" << std::endl;
    TrackCategorizer trackCategorizer(event, setup, pSet);
	
    std::cout << "test2" << std::endl;
    vertexCategorizer.evaluate(pSet, trackCategorizer);

    std::cout << "test3" << std::endl;
    std::vector<VertexMCInformation> vertex_infos = vertexCategorizer.returnAnalyzedVertices();

    std::cout << "test4" << std::endl;
    std::auto_ptr<std::vector<VertexMCInformation> > result(&vertex_infos);

    std::cout << result->size() << std::endl;
    std::cout << "test5" << std::endl;
    event.put(result);
    
    std::cout << "test6" << std::endl;
}


void
VertexCategorizerProducer::beginRun(const edm::Run& run, const edm::EventSetup& setup)
{
    // Get the particles table.
    // setup.getData( pdt_ );
}

void
VertexCategorizerProducer::beginJob()
{

}


DEFINE_FWK_MODULE(VertexCategorizerProducer);
