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

#include "HepPDT/ParticleID.hh"

#include "TH2F.h"

//
// class decleration
//

class VertexCategorizerTest : public edm::EDAnalyzer
{
public:

    explicit VertexCategorizerTest(const edm::ParameterSet&);

private:

    struct G4
    {
        enum Process
        {
            Undefined = 0,
            Unknown,
            Primary,
            Hadronic,
            Decay,
            Compton,
            Annihilation,
            EIoni,
            HIoni,
            MuIoni,
            Photon,
            MuPairProd,
            Conversions,
            EBrem,
            SynchrotronRadiation,
            MuBrem,
            MuNucl
        };
    };

    virtual void beginRun(const edm::Run&,const edm::EventSetup&);
    virtual void beginJob() ;
    virtual void analyze(const edm::Event&, const edm::EventSetup&);

    // Member data

    // VertexCategorizer vertexClassifier_;

    const edm::ParameterSet pSet;

    TH1F *geantProcessTypes, *cmsProcessTypes;
    TH1F *bRhoTight, *bRhoMed, *bRhoLoose, *niRhoTight, *niRhoMed, *niRhoLoose;
    TH2F *bRho2D, *niRho2D;

    // edm::ESHandle<ParticleDataTable> pdt_;

};


VertexCategorizerTest::VertexCategorizerTest(const edm::ParameterSet& config) :
    // vertexClassifier_(config),
    pSet(config)
{
    edm::Service<TFileService> fs;

    geantProcessTypes = fs->make<TH1F>("GeantTypes", "Geant Process Types", 300, -0.5, 300 - 0.5);
    cmsProcessTypes = fs->make<TH1F>("CMSTypes", "CMS Process Types", 300, -0.5, 300 - 0.5);
    bRhoTight = fs->make<TH1F>("bRhoTight", "B Rho (Tight)", 240, 0., 12.);
    bRhoMed = fs->make<TH1F>("bRhoMed", "B Rho (Med)", 240, 0., 12.);
    bRhoLoose = fs->make<TH1F>("bRhoLoose", "B Rho (Loose)", 240, 0., 12.);
    niRhoTight = fs->make<TH1F>("niRhoTight", "NI Rho (Tight)", 240, 0., 12.);
    niRhoMed = fs->make<TH1F>("niRhoMed", "NI Rho (Med)", 240, 0., 12.);
    niRhoLoose = fs->make<TH1F>("niRhoLoose", "NI Rho (Loose)", 240, 0., 12.);
    bRho2D = fs->make<TH2F>("bRho2D", ";Vertex Rho;b fraction", 240, 0., 12., 130, 0., 1.3);
    niRho2D = fs->make<TH2F>("niRho2D", ";Vertex Rho;ni fraction", 240, 0., 12., 130, 0., 1.3);



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
        double total_weight = 0.;
        double b_weight = 0.;
        double ni_weight = 0.;
        for (const TrackMCInformation & trackInfo : vertInfo.analyzedTracks)
        {
            geantProcessTypes->Fill(trackInfo.geantProcessType);
            cmsProcessTypes->Fill(trackInfo.cmsProcessType);

            total_weight++;
            HepPDT::ParticleID particleID(std::abs(trackInfo.motherParticleID));
            if (particleID.hasBottom())
                b_weight++;
            else if (trackInfo.cmsProcessType == G4::Hadronic)
                ni_weight++;
        }

        double b_frac = (total_weight) ? b_weight/total_weight : -1.;
        double ni_frac = (total_weight) ? ni_weight/total_weight : -1.;

        double vertRho = std::sqrt(vertInfo.vertexRef->vx()*vertInfo.vertexRef->vx() + vertInfo.vertexRef->vy()*vertInfo.vertexRef->vy());

        if (b_frac > 0.9)
            bRhoTight->Fill(vertRho);
        else if (b_frac > 0.5)
            bRhoMed->Fill(vertRho);
        else if (b_frac > 0.1)
            bRhoLoose->Fill(vertRho);

        if (ni_frac > 0.9)
            niRhoTight->Fill(vertRho);
        else if (ni_frac > 0.5)
            niRhoMed->Fill(vertRho);
        else if (ni_frac > 0.1)
            niRhoLoose->Fill(vertRho);

        bRho2D->Fill(vertRho, b_frac);
        niRho2D->Fill(vertRho, ni_frac);


    }


	// for (edm::RefToBaseVector<reco::Vertex>::const_iterator iVertex = vertRefVec.begin(); iVertex < vertRefVec.end(); ++iVertex)
	// {
	// 	vertexClassifier_.evaluate(*iVertex);
	// }

    // Get a constant reference to the track history associated to the classifier
    // VertexHistory const & tracer = vertexClassifier_.history();

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
