// -*- C++ -*-
//
// Package:    IvfAnalyzer
// Class:      IvfAnalyzer
// 
/**\class IvfAnalyzer IvfAnalyzer.cc SimTracker/VertexHistory/src/IvfAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Dominik Nowatschin,68/102,2978,
//         Created:  Mon Jan 13 14:35:23 CET 2014
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "TH2.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"

#include <math.h>


//
// class declaration
//

class IvfAnalyzer : public edm::EDAnalyzer {
   public:
      explicit IvfAnalyzer(const edm::ParameterSet&);
      ~IvfAnalyzer();

// //       static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
//       virtual void beginJob() ;
      virtual void endJob() ;

//       virtual void beginRun(edm::Run const&, edm::EventSetup const&);
//       virtual void endRun(edm::Run const&, edm::EventSetup const&);
//       virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
//       virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);



      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      
      unsigned int maxCount_;
      
      edm::ParameterSet parSet_;
      
      bool data_;
      
      TString title_;
//       recoVert_, sec_vert_NonB_;
      
      TH2D *SVhistoRhoZ, *SVhistoXY, *SVhistoRhoPhi, *SVhistoParXY, *SVhistoParRhoZ, *SVhistoParRhoPhi, *TVhistoRhoZ, *TVhistoXY, *TVhistoRhoPhi;
      TH1D *SVhistoRho, *SVhistoParRho, *TVhistoRho;


      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
IvfAnalyzer::IvfAnalyzer(const edm::ParameterSet& pSet) :
	maxCount_(pSet.getParameter<unsigned int>("maxCount")),
	parSet_(pSet.getParameter<edm::ParameterSet>("plotConfig")),
	data_(pSet.getParameter<bool>("data")),
	title_(pSet.getParameter<std::string>("title"))
	
// 	recoVert_(pSet.getParameter<edm::InputTag>("secVertB")),
// 	sec_vert_NonB_(pSet.getParameter<edm::InputTag>("secVertNonB"))
{
   edm::Service<TFileService> fs;
   SVhistoRhoZ = fs->make<TH2D>("secVerticesRhoZ", "Secondary Vertices Z-Rho", 560, -70, 70, 300, 0, 30);
   SVhistoXY = fs->make<TH2D>("secVerticesXY", "Secondary Vertices X-Y",  600, -30, 30, 600, -30, 30);
   SVhistoRhoPhi = fs->make<TH2D>("secVerticesRhoPhi", "Secondary Vertices Phi-Rho",  300, -3.15, 3.15, 300, 0, 30);
   SVhistoRho = fs->make<TH1D>("secVerticesRho", "Secondary Vertices Rho Projection", 300, 0, 30);
   if (data_)
   {
       SVhistoParRhoZ = fs->make<TH2D>("secVerticesParRhoZ", "(Parametrized) Secondary Vertices Z-Rho", 560, -70, 70, 300, 0, 30);
       SVhistoParXY = fs->make<TH2D>("secVerticesParXY", "(Parametrized) Secondary Vertices X-Y",  600, -30, 30, 600, -30, 30);
       SVhistoParRhoPhi = fs->make<TH2D>("secVerticesParRhoPhi", "(Parametrized) Secondary Vertices Phi-Rho",  300, -3.15, 3.15, 300, 0, 30);
       SVhistoParRho = fs->make<TH1D>("secVerticesParRho", "(Parametrized) Secondary Vertices Rho Projection", 300, 0, 30);
   }
   if (!data_)
   {
       TVhistoRhoZ = fs->make<TH2D>("trackVerticesRhoZ", "Tracking Vertices Z-Rho", 560, -70, 70, 300, 0, 30);
       TVhistoXY = fs->make<TH2D>("trackVerticesXY", "Tracking Vertices X-Y",  600, -30, 30, 600, -30, 30);
       TVhistoRhoPhi = fs->make<TH2D>("trackVerticesRhoPhi", "Tracking Vertices Phi-Rho",  300, -3.15, 3.15, 300, 0, 30);
       TVhistoRho = fs->make<TH1D>("trackVerticesRho", "Tracking Vertices Rho Projection", 300, 0, 30);
   }

}


IvfAnalyzer::~IvfAnalyzer()
{
}


//
// member functions
//

// ------------ method called for each event  ------------
void
IvfAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   
   unsigned int counter = 0;
   unsigned int track_counter = 0;
   
   edm::InputTag recoVert_ = parSet_.getParameter<edm::InputTag>("secVertB");
   Handle< edm::View<reco::Vertex> > recoVert;
   iEvent.getByLabel(recoVert_, recoVert);
   
   Handle<TrackingParticle::TrackingVertexCollection> trackVert;
   iEvent.getByLabel(recoVert_, trackVert);
   
   TString titlesRhoZ = title_+";Z;Rho";
   TString titlesXY = title_+";X;Y";
   TString titlesRhoPhi = title_+";Phi;Rho";
   TString titlesRho = title_+";Rho;No. of Events";
   
   TString titlesRhoZPar = title_+" (Param.);Z;Rho";
   TString titlesXYPar = title_+" (Param.);X;Y";
   TString titlesRhoPhiPar = title_+" (Param.);Phi;Rho";
   TString titlesRhoPar = title_+" (Param.);Rho;No. of Events";
   
   TString titlesRhoZTrack = title_+" (Sim);Z;Rho";
   TString titlesXYTrack = title_+" (Sim);X;Y";
   TString titlesRhoPhiTrack = title_+" (Sim);Phi;Rho";
   TString titlesRhoTrack = title_+" (Sim);Rho;No. of Events";
   
   SVhistoRhoZ->SetTitle(titlesRhoZ);
   SVhistoXY->SetTitle(titlesXY);
   SVhistoRhoPhi->SetTitle(titlesRhoPhi);
   SVhistoRho->SetTitle(titlesRho);
   
   if (data_)
   {       
       SVhistoParRhoZ->SetTitle(titlesRhoZPar);
       SVhistoParXY->SetTitle(titlesXYPar);
       SVhistoParRhoPhi->SetTitle(titlesRhoPhiPar);
       SVhistoParRho->SetTitle(titlesRhoPar);
   }
   
   if (!data_)
   {       
       TVhistoRhoZ->SetTitle(titlesRhoZTrack);
       TVhistoXY->SetTitle(titlesXYTrack);
       TVhistoRhoPhi->SetTitle(titlesRhoPhiTrack);
       TVhistoRho->SetTitle(titlesRhoTrack);
   }
   
   for (edm::View<reco::Vertex>::const_iterator ivertex = recoVert->begin(); ivertex != recoVert->end() && counter < maxCount_; ++ivertex)
   {
     counter++;
     double secvert_rho = ivertex->position().rho();
     double secvert_phi = ivertex->position().phi();
     double secvert_z = ivertex->position().z();
     double secvert_y = ivertex->position().y();
     double secvert_x = ivertex->position().x();
     
     //=======AXIS PARAMETRIZATION==================
     
     double secvert_x_par = ivertex->position().x()-(0.086-0.0007*secvert_z);
     double secvert_y_par = ivertex->position().y()-(-0.197+0.0007*secvert_z);
     double secvert_rho_par = sqrt(secvert_x_par*secvert_x_par+secvert_y_par*secvert_y_par);
     double secvert_phi_par = atan(secvert_x_par/secvert_y_par);
     
     SVhistoRhoZ->Fill(secvert_z, secvert_rho);
     SVhistoXY->Fill(secvert_x, secvert_y);
     SVhistoRhoPhi->Fill(secvert_phi, secvert_rho);
     SVhistoRho->Fill(secvert_rho);
     
     
     if (data_)
     {
	 SVhistoParRhoZ->Fill(secvert_z, secvert_rho_par);
	 SVhistoParXY->Fill(secvert_x_par, secvert_y_par);
	 SVhistoParRhoPhi->Fill(secvert_phi_par, secvert_rho_par);
	 SVhistoParRho->Fill(secvert_rho_par);
     }    
     
   }
   
   if (parSet_.exists("secVertNonB"))
   {   
     edm::InputTag sec_vert_NonB_ = parSet_.getParameter<edm::InputTag>("secVertNonB");
     Handle< edm::View<reco::Vertex> > sec_vert_NonB;
     iEvent.getByLabel(sec_vert_NonB_, sec_vert_NonB);

     for (edm::View<reco::Vertex>::const_iterator ivertex = sec_vert_NonB->begin(); ivertex != sec_vert_NonB->end() && counter < maxCount_; ++ivertex)
     {
	 counter++;
	 double secvert_rho = ivertex->position().rho();
	 double secvert_phi = ivertex->position().phi();
	 double secvert_z = ivertex->position().z();
	 double secvert_y = ivertex->position().y();
	 double secvert_x = ivertex->position().x();
	 
	 //=======AXIS PARAMETRIZATION==================
	 
	 double secvert_x_par = ivertex->position().x()-(0.086-0.0007*secvert_z);
	 double secvert_y_par = ivertex->position().y()-(-0.197+0.0007*secvert_z);
	 double secvert_rho_par = sqrt(secvert_x_par*secvert_x_par+secvert_y_par*secvert_y_par);
	 double secvert_phi_par = atan(secvert_x_par/secvert_y_par);
	 
	 SVhistoRhoZ->Fill(secvert_z, secvert_rho);
	 SVhistoXY->Fill(secvert_x, secvert_y);
	 SVhistoRhoPhi->Fill(secvert_phi, secvert_rho);
	 SVhistoRho->Fill(secvert_rho);
	 
	 SVhistoRhoZ->SetTitle(titlesRhoZ);
	 SVhistoXY->SetTitle(titlesXY);
	 SVhistoRhoPhi->SetTitle(titlesRhoPhi);
	 SVhistoRho->SetTitle(titlesRho);
	 if (data_)
	 {
	     SVhistoParRhoZ->Fill(secvert_z, secvert_rho_par);
	     SVhistoParXY->Fill(secvert_x_par, secvert_y_par);
	     SVhistoParRhoPhi->Fill(secvert_phi_par, secvert_rho_par);
	     SVhistoParRho->Fill(secvert_rho_par);
	     
	     SVhistoParRhoZ->SetTitle(titlesRhoZPar);
	     SVhistoParXY->SetTitle(titlesXYPar);
	     SVhistoParRhoPhi->SetTitle(titlesRhoPhiPar);
	     SVhistoParRho->SetTitle(titlesRhoPar);
	 } 
     }
   }
   
   if (!data_)
   {
//        std::cout << "Start filling TrackingVertexHistos." << std::endl;
      for (TrackingParticle::TrackingVertexCollection::const_iterator ivertex = trackVert->begin(); ivertex != trackVert->end() && track_counter < maxCount_; ++ivertex)
      {
	  track_counter++;
	  double secvert_rho = ivertex->position().rho();
	  double secvert_phi = ivertex->position().phi();
	  double secvert_z = ivertex->position().z();
	  double secvert_y = ivertex->position().y();
	  double secvert_x = ivertex->position().x();
	  
	  //=======AXIS PARAMETRIZATION==================
	  
// 	  double secvert_x_par = ivertex->position().x()-(0.086-0.0007*secvert_z);
// 	  double secvert_y_par = ivertex->position().y()-(-0.197+0.0007*secvert_z);
// 	  double secvert_rho_par = sqrt(secvert_x_par*secvert_x_par+secvert_y_par*secvert_y_par);
// 	  double secvert_phi_par = acos(secvert_x_par/secvert_rho_par);
	  
	  TVhistoRhoZ->Fill(secvert_z, secvert_rho);
	  TVhistoXY->Fill(secvert_x, secvert_y);
	  TVhistoRhoPhi->Fill(secvert_phi, secvert_rho);
	  TVhistoRho->Fill(secvert_rho);
	  
// 	  std::cout << "	TV filled!" << std::endl;
	       
      }
   }
   
} 


// // ------------ method called once each job just before starting event loop  ------------
// void 
// IvfAnalyzer::beginJob()
// {
// }
// 
// // ------------ method called once each job just after ending the event loop  ------------
void 
IvfAnalyzer::endJob() 
{
  
}
// 
// // ------------ method called when starting to processes a run  ------------
// void 
// IvfAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
// {
// }
// 
// // ------------ method called when ending the processing of a run  ------------
// void 
// IvfAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
// {
// }
// 
// // ------------ method called when starting to processes a luminosity block  ------------
// void 
// IvfAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
// {
// }
// 
// // ------------ method called when ending the processing of a luminosity block  ------------
// void 
// IvfAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
// {
// }
// 
// // ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
// void
// IvfAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
//   //The following says we do not know what parameters are allowed so do no validation
//   // Please change this to state exactly what you do use, even if it is no parameters
//   edm::ParameterSetDescription desc;
//   desc.setUnknown();
//   descriptions.addDefault(desc);
// }

//define this as a plug-in
DEFINE_FWK_MODULE(IvfAnalyzer);
