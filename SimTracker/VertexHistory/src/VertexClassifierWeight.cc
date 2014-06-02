#include <math.h>
#include <cstdlib>
#include <iostream>

#include "SimTracker/VertexHistory/interface/VertexClassifierWeight.h"

#include "HepPDT/ParticleID.hh"

#include "SimDataFormats/TrackingAnalysis/interface/ParticleBase.h"


VertexClassifierWeight::VertexClassifierWeight(const edm::ParameterSet & config) : HistoryBaseOwn(), param_(config.getParameter<edm::ParameterSet>("trackingParticleSelector"))
{
  
  //! declare here possible variables that you want to set from the config file and want to remain unchanged throughout the whole CYCLE
  
  status(2);
  
  
}

VertexClassifierWeight::~VertexClassifierWeight()
{
}
 

void VertexClassifierWeight::newEvent(edm::RefToBaseVector<reco::Track> recoTracks, edm::RefVector<TrackingParticleCollection> TPCollection, const edm::Event & event, const edm::EventSetup& setup, bool debugMessage)
{
  //! declare here possible variables that you want to remain unchanged for each EVENT
  
  HistoryBaseOwn::newEvent(setup);
  
  debugMsg(debugMessage);
  
  
  //! declare trackAssociator here or in constructor?
  edm::ESHandle<TrackAssociatorBase> trackAssociator;
  setup.get<TrackAssociatorRecord>().get("TrackAssociatorByHits", trackAssociator);
  
  trackRecoToSim_ = trackAssociator->associateRecoToSim(recoTracks, TPCollection, &event);
  
  selector_ = TrackingParticleSelector(
    param_.getParameter<double>("ptMinTP"),
    param_.getParameter<double>("minRapidityTP"),
    param_.getParameter<double>("maxRapidityTP"),
    param_.getParameter<double>("tipTP"),
    param_.getParameter<double>("lipTP"),
    param_.getParameter<int>("minHitTP"),
    param_.getParameter<bool>("signalOnlyTP"),
    param_.getParameter<bool>("chargedOnlyTP"),
    param_.getParameter<bool>("stableOnlyTP"),
    param_.getParameter<std::vector<int> >("pdgIdTP")
  );
  
}

void VertexClassifierWeight::evaluate(const reco::Vertex& recoVertex)
{
  //! declare variables here that you want to remain unchanged for each VERTEX
  
  if (debugMsg_)
    std::cout << "  RecoVertex position: " << recoVertex.position().x() << " " << recoVertex.position().y() << " " << recoVertex.position().z() << " " << std::endl << std::endl;
  
  reset(); /// reset the flag and matches containers
  
  double recoDaughterWeight = 0.;
  
  bool isFake = true;
  
  double Hadronic = 0.;
  double Unknown = 0.;
  double Undefined = 0.;
  double Decay = 0.;
  double GeantPrimary = 0.;
  double Compton = 0.;
  double Annihilation = 0.;
  double EIoni = 0.;
  double HIoni = 0.;
  double MuIoni = 0.;
  double Photon = 0.;
  double MuPairProd = 0.;
  double Conversions = 0.;
  double EBrem = 0.;
  double SynchrotronRadiation = 0.;
  double MuBrem = 0.;
  double MuNucl = 0.;
  
  double HadronicWeight = 0.;
  double UnknownWeight = 0.;
  double UndefinedWeight = 0.;
  double DecayWeight = 0.;
  double GeantPrimaryWeight = 0.;
  double ComptonWeight = 0.;
  double AnnihilationWeight = 0.;
  double EIoniWeight = 0.;
  double HIoniWeight = 0.;
  double MuIoniWeight = 0.;
  double PhotonWeight = 0.;
  double MuPairProdWeight = 0.;
  double ConversionsWeight = 0.;
  double EBremWeight = 0.;
  double SynchrotronRadiationWeight = 0.;
  double MuBremWeight = 0.;
  double MuNuclWeight = 0.;
  
//   TrackingVertexRef trackingVertexEvaluate;
  
  /// go through all daughter tracks and set the flags according to the corresponding process of the best matching trackingParticle
  
  int recoDaughterCounter = 0;
  
  if (debugMsg_)
    std::cout << "  ====LOOP OVER ALL RECO DAUGHTERS====" << std::endl << std::endl;
  
  for (reco::Vertex::trackRef_iterator iVertDaughter = recoVertex.tracks_begin(); iVertDaughter != recoVertex.tracks_end(); ++iVertDaughter)
  {
    recoDaughterCounter++;
    if (debugMsg_)
      std::cout << "  RecoDaughter No. " << recoDaughterCounter  << std::endl;
    
    // TRACK QUALITY, WHERE TO GET FROM?? in original vertexAssociator is obtained from config ParameterSet which again is coming from the EventSetup upon initialisation of the vertexAssociator; maybe initialize vertexAssociator and then get trackQuality_ from there? but is private member...
    // UPDATE: don't need because tracks have already been selected!!
    //       if ( !(*iVertDaughter)->quality(trackQuality_) ) continue;
    
    recoDaughterWeight += recoVertex.trackWeight(*iVertDaughter);
    
    /// Check for association for the given RecoDaughter
    if ( trackRecoToSim_.numberOfAssociations(*iVertDaughter) > 0 )
    {
      isFake = false;
      std::vector<std::pair<TrackingParticleRef,double> > associations = trackRecoToSim_[*iVertDaughter];
      
      if (debugMsg_)
	std::cout << "    Qualities of associated Tracking Particles: ";
      
      //! only use the best association instead of loop over all associations in next step?
//       for (std::vector<std::pair<TrackingParticleRef,double> >::const_iterator association = associations.begin(); association != associations.end(); ++association )
//       {
// 	// Get a reference to parent vertex of TrackingParticle associated to the RecoDaughter
// 	TrackingVertexRef trackingVertex = association->first->parentVertex();
// 	// Store matched RecoDaughter to the trackingVertex
// 	matches_[trackingVertex].first += recoVertex.trackWeight(*iVertDaughter);
// 	matches_[trackingVertex].second++;
//       }
      
      /// create a map of the vector containing the TrackingParticleRefs associated with the RecoTrack sorted according to the quality of the association
      std::map<double, TrackingParticleRef> associationsSorted;
      for (unsigned int i=0; i < associations.size(); ++i)
      {
	if (debugMsg_)
	  std::cout << associations[i].second << " ";
	associationsSorted[associations[i].second] = associations[i].first;
      }
      if (debugMsg_)
	std::cout << std::endl;
      
      
      // Get a reference to parent vertex of TrackingParticle associated to the RecoDaughter
      TrackingVertexRef trackingVertex = associationsSorted.rbegin()->second->parentVertex();
      // Store matched RecoDaughter to the trackingVertex
      matches_[trackingVertex].first += recoVertex.trackWeight(*iVertDaughter);
      matches_[trackingVertex].second++;
      
      if (debugMsg_)
	std::cout << "    TrackQuality of best association: " << associationsSorted.rbegin()->first << std::endl;
            
      /// take only the first association with the highest quality
      unsigned short process = 0;
      
      if(!associationsSorted.rbegin()->second->trackPSimHit().empty())
      {
	if (debugMsg_)
	  std::cout << "    Process Type of best association: " << associationsSorted.rbegin()->second->pSimHit_begin()->processType() << std::endl << std::endl;
	process = associationsSorted.rbegin()->second->pSimHit_begin()->processType();
	
      }
      else
	if (debugMsg_)
	  std::cout << "    Associated Tracking Particle has no SimHits!" << std::endl << std::endl;
      if (process == G4::Hadronic) Hadronic += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::Unknown) Unknown += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::Undefined) Undefined += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::Decay) Decay += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::Primary) GeantPrimary += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::Compton) Compton += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::Annihilation) Annihilation += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::EIoni) EIoni += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::HIoni) HIoni += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::MuIoni) MuIoni += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::Photon) Photon += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::MuPairProd) MuPairProd += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::EBrem) EBrem += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::SynchrotronRadiation) SynchrotronRadiation += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::MuBrem) MuBrem += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::Conversions) Conversions += recoVertex.trackWeight(*iVertDaughter);
      if (process == G4::MuNucl) MuNucl += recoVertex.trackWeight(*iVertDaughter);
    }
    
            //! TODO: include "NoProcess" weight in case 'if(!associationsSorted.rbegin()->second->trackPSimHit().empty())' fails or "NoAssociation" weight in case 'if ( trackRecoToSim_.numberOfAssociations(*iVertDaughter) > 0 )' fails or make a combined weight of these two
      
  }
    
  
  if (debugMsg_)
    std::cout << "  ====CHECK IF RECO VERTEX IS FAKE====" << std::endl << std::endl;
  
  if (isFake)
  {
    if (debugMsg_)
      std::cout << "  Vertex is Fake!" << std::endl << std::endl;
    thisProcessFlags_[Fake] = 1.;
    return;
  }
  
  recoDaughterWeight_ = recoDaughterWeight;
  
  if (recoDaughterWeight != 0)
  {
    HadronicWeight = Hadronic/recoDaughterWeight;
    UnknownWeight = Unknown/recoDaughterWeight;
    UndefinedWeight = Undefined/recoDaughterWeight;
    DecayWeight = Decay/recoDaughterWeight;
    GeantPrimaryWeight = GeantPrimary/recoDaughterWeight;
    ComptonWeight = Compton/recoDaughterWeight;
    AnnihilationWeight = Annihilation/recoDaughterWeight;
    EIoniWeight = EIoni/recoDaughterWeight;
    HIoniWeight = HIoni/recoDaughterWeight;
    MuIoniWeight = MuIoni/recoDaughterWeight;
    PhotonWeight = Photon/recoDaughterWeight;
    MuPairProdWeight = MuPairProd/recoDaughterWeight;
    ConversionsWeight = Conversions/recoDaughterWeight;
    EBremWeight = EBrem/recoDaughterWeight;
    SynchrotronRadiationWeight = SynchrotronRadiation/recoDaughterWeight;
    MuBremWeight = MuBrem/recoDaughterWeight;
    MuNuclWeight = MuNucl/recoDaughterWeight;
    
    thisProcessFlags_[HadronicProcess] = HadronicWeight;
    thisProcessFlags_[UnknownProcess] = UnknownWeight;
    thisProcessFlags_[UndefinedProcess] = UndefinedWeight;
    thisProcessFlags_[DecayProcess] = DecayWeight;
    thisProcessFlags_[GeantPrimaryProcess] = GeantPrimaryWeight;
    thisProcessFlags_[ComptonProcess] = ComptonWeight;
    thisProcessFlags_[AnnihilationProcess] = AnnihilationWeight;
    thisProcessFlags_[EIoniProcess] = EIoniWeight;
    thisProcessFlags_[HIoniProcess] = HIoniWeight;
    thisProcessFlags_[MuIoniProcess] = MuIoniWeight;
    thisProcessFlags_[PhotonProcess] = PhotonWeight;
    thisProcessFlags_[MuPairProdProcess] = MuPairProdWeight;
    thisProcessFlags_[ConversionsProcess] = ConversionsWeight;
    thisProcessFlags_[EBremProcess] = EBremWeight;
    thisProcessFlags_[SynchrotronRadiationProcess] = SynchrotronRadiationWeight;
    thisProcessFlags_[MuBremProcess] = MuBremWeight;
    thisProcessFlags_[MuNuclProcess] = MuNuclWeight;
  }
  
  bool deactivateMsg = false;
  
  if (HadronicWeight > 0.1 && !debugMsg_)
  {
      deactivateMsg = true;
      debugMsg_ = true;
      std::cout << "  RecoVertex position: " << recoVertex.position().x() << " " << recoVertex.position().y() << " " << recoVertex.position().z() << " " << std::endl << std::endl;
  }
  
  /// get the trackingVertex matching this recoVertex (taken from VertexAssociatorByTracks)
  
  if (debugMsg_)
    std::cout << "  ====GET THE BEST MATCHING TRACKING VERTEX====" << std::endl << std::endl;
  
//   trackingVertexEvaluate = getBestTrackingVertex(matches_, recoDaughterWeight);
  
  /// if the vertex comes from a decay check which one
  
  if (debugMsg_)
    std::cout << "  ====CHECK SOURCE OF ASSOCIATED TRACKING VERTICES====" << std::endl << std::endl;
    
  ///=====DIFFERENT APPROACH NOW: GO THROUGH ALL ASSOCIATED TRACKING VERTICES=====
  
    /// check for same genVertices?
//   GenVertexTrailHelper currentVertexGenVerticesHelper;
    
    for (std::map<TrackingVertexRef, std::pair<double, std::size_t> >::const_iterator iTrackingVertex = matches_.begin(); iTrackingVertex != matches_.end(); ++iTrackingVertex)
    {
	double weight = iTrackingVertex->second.first/recoDaughterWeight;
	TrackingVertexRef const & trackingVertex = iTrackingVertex->first;
	if (debugMsg_)
	{
	    std::cout << "  Check Tracking Vertex with weight " << weight << std::endl;
	    std::cout << "  Tracking Vertex Position: " << trackingVertex->position().x() << " " << trackingVertex->position().y() << " " <<  trackingVertex->position().z() << std::endl << std::endl;
	    
	}
	
	if (!HistoryBaseOwn::evaluate(trackingVertex, thisProcessFlags_, 0, weight) )
	{
	    if (debugMsg_) {
		std::cout << "  Vertex is Fake!" << std::endl << std::endl;
	    }
	    thisProcessFlags_[Fake] = 1.;
	}
    }
  
  if (debugMsg_)
  {
    std::cout << "  ThisProcessWeights: Fake | PrimaryVertex | GeantPrimary | Hadronic | Unknown | Undefined | Decay | Proton | BWeak | CWeak | Kshort" << std::endl;
    std::cout << "  " << thisProcessFlags_[Fake] << "  " << thisProcessFlags_[PrimaryProcess] << "  " << thisProcessFlags_[GeantPrimaryProcess] << " " << thisProcessFlags_[HadronicProcess] << " " << thisProcessFlags_[UnknownProcess] << " " << thisProcessFlags_[UndefinedProcess] << " " << thisProcessFlags_[DecayProcess] << "  " << thisProcessFlags_[ProtonDecay] << " " << thisProcessFlags_[BWeakDecay] << " " << thisProcessFlags_[CWeakDecay] << " " << thisProcessFlags_[KsDecay] << " " << std::endl << std::endl;
  }
  
  /// check out the history of the first tracking particle of the recoVertex
  
  if (debugMsg_)
    std::cout << "  ====NOW CHECK THE HISTORY OF THE VERTEX====" << std::endl << std::endl;
  
  for (std::map<TrackingVertexRef, std::pair<double, std::size_t> >::const_iterator iTrackingVertex = matches_.begin(); iTrackingVertex != matches_.end(); ++iTrackingVertex)
  {
      double weight = iTrackingVertex->second.first/recoDaughterWeight;
      TrackingVertexRef const & trackingVertex = iTrackingVertex->first;
      if (debugMsg_)
      {
	  std::cout << "  Check Tracking Vertex with weight " << weight << std::endl;
	  std::cout << "  Tracking Vertex Position: " << trackingVertex->position().x() << " " << trackingVertex->position().y() << " " <<  trackingVertex->position().z() << std::endl << std::endl;
	  
      }
      
      if (!HistoryBaseOwn::evaluate(trackingVertex, historyFlags_, 1, weight) )
      {
	  if (debugMsg_)
	      std::cout << "  Error in history!" << std::endl << std::endl;
      }
  }
  
  if (debugMsg_)
  {
      std::cout << "  ThisProcessWeights: Fake | PrimaryVertex | GeantPrimary | Hadronic | Unknown | Undefined | Decay | Proton | BWeak | CWeak | Kshort" << std::endl;
      std::cout << "  " << historyFlags_[Fake] << "  " << historyFlags_[PrimaryProcess] << "  " << historyFlags_[GeantPrimaryProcess] << " " << historyFlags_[HadronicProcess] << " " << historyFlags_[UnknownProcess] << " " << historyFlags_[UndefinedProcess] << " " << historyFlags_[DecayProcess] << "  " << historyFlags_[ProtonDecay] << " " << historyFlags_[BWeakDecay] << " " << historyFlags_[CWeakDecay] << " " << historyFlags_[KsDecay] << " " << std::endl << std::endl;
  }

      
  
//   if (!trackingVertexEvaluate.isNull())
//   {
//     
//     if (debugMsg_)
//       std::cout << "  HistoryBaseOwn::evaluate -> create simParticleTrail and genParticleTrail" << std::endl;
//     if (HistoryBaseOwn::evaluate(trackingVertexEvaluate, historyFlags_, -1))
//     {
//       int stepHistory = 0;
//       
//       if (debugMsg_)
//       {
// 	std::cout << std::endl;
// 	std::cout << "  TrackingVertex SimHistory size: " << simParticleTrail_.size() << std::endl;
// 	std::cout << "  TrackingVertex GenHistory size: " << genParticleTrail_.size() << std::endl << std::endl;
//       }
//       
//       if (debugMsg_)
// 	std::cout << "  Simulation History: PDGID | Travel Length" << std::endl;  
//       if (simParticleTrail_.size() > 0)
// 	processesAtSimulation(stepHistory, trackingVertexEvaluate);
//       
//       if (debugMsg_)
// 	std::cout << std::endl << "  Generator History: PDGID | Travel Length" << std::endl;
//       
//       if (genParticleTrail_.size() > 0)
// 	processesAtGenerator(stepHistory, trackingVertexEvaluate);
//       
//       if (debugMsg_)
// 	std::cout << std::endl << "  History Finished!" << std::endl << std::endl << std::endl;
//       
//       if (debugMsg_)
//       {
// 	std::cout << "  HistoryWeights: Fake | GeantPrimary | Hadronic | Unknown | Undefined | Decay | BWeak | CWeak | Kshort" << std::endl;
// 	std::cout << "  " << historyFlags_[Fake] << "  " << historyFlags_[GeantPrimaryProcess] << " " << historyFlags_[HadronicProcess] << " " << historyFlags_[UnknownProcess] << " " << historyFlags_[UndefinedProcess] << " " << historyFlags_[DecayProcess] << " " << historyFlags_[BWeakDecay] << " " << historyFlags_[CWeakDecay] << " " << historyFlags_[KsDecay] << " " << std::endl << std::endl;
//       }
//     }
//     else
//       if (debugMsg_)    
// 	std::cout << "  Tracking Vertex could not be evaluated!" << std::endl << std::endl;
//   }
//   else
//     if (debugMsg_)    
//       std::cout << "  No Tracking Vertex Found!" << std::endl << std::endl;

    if (deactivateMsg)
	debugMsg_ = false;
    
    return;

}


//! CHECK OUT: (class) HepPDT::ParticleID, (class) ParticleData, ParticleData::lifetime() 














// void VertexClassifierWeight::processesAtGenerator(int & stepHistory, TrackingVertexRef & evaluatedVertex)
// {
// //   HepMC::GenVertex * previousVertex = (*genParticleTrail_.begin())->production_vertex();
//   // Loop over the generated particles
//   for (HistoryBaseOwn::GenParticleTrail::const_iterator iparticle = genParticleTrail_.begin(); iparticle != genParticleTrail_.end(); ++iparticle)
//   {
//     int pdgid = 0;
//     double travelLength = -1;
//     
//     // Get the source vertex for the particle
//     HepMC::GenVertex * productionVertex = (*iparticle)->production_vertex();
//     
//     // Get the pointer to the vertex by removing the const-ness (no const methos in HepMC::GenVertex)
//     // HepMC::GenVertex * vertex = const_cast<HepMC::GenVertex *>(*ivertex);
//     
//     // Check for a non-null pointer to the production vertex
//     
//     if (productionVertex)
//     {
//       stepHistory++;
//       
//       if (debugMsg_)
// 	std::cout << "  Step No. " << stepHistory << std::endl;
//       
//       // Only case track history will navigate (one in or source particle per vertex)
//       if ( productionVertex->particles_in_size() == 1 )
//       {
// 	// Look at the pdgid of the first "in" particle to the vertex
// 	pdgid = std::abs((*productionVertex->particles_in_const_begin())->pdg_id());
// 	
// 	flagProcess(historyFlags_, pdgid, (double)1/stepHistory);
// 	
//       }
//       
//       HepMC::ThreeVector thisVertPos = productionVertex->point3d();
//       TrackingVertex::LorentzVector evalVertPos = evaluatedVertex->position();
//       HepMC::ThreeVector flightVector;
//       flightVector.set((evalVertPos.x()*10)-thisVertPos.x(), (evalVertPos.y()*10)-thisVertPos.y(), (evalVertPos.z()*10)-thisVertPos.z());
//       travelLength = flightVector.r()*0.1;
//       
// //       previousVertex = productionVertex;
// 
//     }
//     
//     if (debugMsg_)
//       std::cout << "    " << pdgid << " " << travelLength << std::endl;
//   }
//   
//   return;
//   
// }
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// void VertexClassifierWeight::processesAtSimulation(int & stepHistory, TrackingVertexRef & evaluatedVertex)
// {
//   
//   // Loop over the simulated particles
//   for (TrackHistory::SimParticleTrail::const_iterator iparticle = simParticleTrail_.begin(); iparticle != simParticleTrail_.end(); ++iparticle)
//   {    
//     
//     // pdgid of the real source parent vertex
//     int pdgid = -1;
//     double travelLength = -1;
//     
//     // Get a reference to the TP's parent vertex
//     TrackingVertexRef const & parentVertex = (*iparticle)->parentVertex();
//     
// //     std::cout << "  Test1" << std::endl;
//     
//     // Look for the original source track
//     if ( parentVertex.isNonnull() )
//     {
//       stepHistory++;
//       
//       if (debugMsg_)
// 	std::cout << "  Step No. " << stepHistory << std::endl;
//       
//       // select the original source in case of combined vertices
//       TrackingParticleRef trackingVertexEvaluateSource = getSourceTrack(parentVertex);
//       
// //       std::cout << "  Test2" << std::endl;
//       
//       // Collect the pdgid of the original source track
//       if ( trackingVertexEvaluateSource.isNonnull() )
// 	pdgid = std::abs(trackingVertexEvaluateSource->pdgId());
//       else
// 	pdgid = 0;
//       
//       // Check for the number of psimhit if different from zero
//       unsigned short process = 0;
//       if (!(*iparticle)->trackPSimHit().empty())
//       {
// 	if (debugMsg_)
// 	  std::cout << "  History TrackingParticle process type: " << (*iparticle)->pSimHit_begin()->processType() << std::endl;
// 	process = (*iparticle)->pSimHit_begin()->processType();	
//       }
//       else
// 	if (debugMsg_)
// 	  std::cout << "  History TrackingParticle has no SimHit!" << std::endl << std::endl;
//       
//       
// //       std::cout << "  Test3" << std::endl;
//     
//       if (process == G4::Decay)
// 	flagProcess(historyFlags_, pdgid, (double)1/stepHistory);
//       
// //       std::cout << "  Test4" << std::endl;
//       
//       TrackingVertex::LorentzVector thisVertPos = parentVertex->position();
//       TrackingVertex::LorentzVector evalVertPos = evaluatedVertex->position();
//       TrackingVertex::LorentzVector flightVector = evalVertPos-thisVertPos;
//       travelLength = flightVector.R();
//       
// //       std::cout << "  Test5" << std::endl;
//       
// //       previousVertex = const_cast<TrackingVertex *>(&(*parentVertex));
//       
// //       std::cout << "  Test6" << std::endl;
//     }
//     else
//       if (debugMsg_)
// 	std::cout << "    History TrackingParticle has now parent Vertex!" << std::endl << std::endl;
//     
//     if (debugMsg_)
//       std::cout << "    " << pdgid << " " << travelLength << std::endl;
//     
//   }
//   
//   return;
//   
// }






std::pair<std::pair<float, int>, TrackingVertexRef> VertexClassifierWeight::getBestTrackingVertex(const std::map<TrackingVertexRef,std::pair<double, std::size_t> > & matches, double recoDaughterWeight) const
{
  
  TrackingVertexRef outputRef;
  float quality;
  int numberDaughters;
  
  std::map<float, std::pair<TrackingVertexRef, int> > bestTrackVertMatch;
  
  if (debugMsg_)
    std::cout << "  Matched Tracking Vertices:" << std::endl;
  
  for ( std::map<TrackingVertexRef,std::pair<double, std::size_t> >::const_iterator match = matches.begin(); match != matches.end(); ++match )
  {
    
    // Getting the TrackingVertex information
    TrackingVertexRef trackingVertex = match->first;
    double matchedDaughterWeight = match->second.first;
    std::size_t matchedDaughterCounter = match->second.second;

    // Count for only reconstructible SimDaughters           
    int simDaughterCounter = 0;
    
    for ( TrackingVertex::tp_iterator simDaughter = trackingVertex->daughterTracks_begin(); simDaughter != trackingVertex->daughterTracks_end(); ++simDaughter )
      if ( selector_(**simDaughter) ) simDaughterCounter++; //! how does selector work?
      
    if (debugMsg_)
    {
      std::cout << "    Tracking Vertex matched daughters & weight: " << matchedDaughterCounter << " " << matchedDaughterWeight/recoDaughterWeight << std::endl;
      std::cout << "    Tracking Vertex daughter PdgIds: ";
      for ( TrackingVertex::tp_iterator simDaughter = trackingVertex->daughterTracks_begin(); simDaughter != trackingVertex->daughterTracks_end(); ++simDaughter )
	  std::cout << (*simDaughter)->pdgId() << " ";
      std::cout << std::endl << "    Tracking Vertex Position: " << trackingVertex->position().x() << " " << trackingVertex->position().y() << " " <<  trackingVertex->position().z() << " " << std::endl;
    }
    
    TrackingParticleRef parentTrack = getSourceTrack(trackingVertex);
    
    ParticleBase::LorentzVector sumDaughterTracks;
    for ( TrackingVertex::tp_iterator simDaughter = trackingVertex->daughterTracks_begin(); simDaughter != trackingVertex->daughterTracks_end(); ++simDaughter )
	sumDaughterTracks = sumDaughterTracks+(*simDaughter)->p4();
    
    if (parentTrack.isNonnull())
    {
	ParticleBase::LorentzVector diffMomentum = sumDaughterTracks-parentTrack->p4();
	if (debugMsg_)
	    std::cout << "      sum(p_out)-p_in size: " << diffMomentum.r() << std::endl << std::endl;
    }
    else
	if (debugMsg_)
	    std::cout << "    Tracking Vertex has no parent Tracking Particle" << std::endl << std::endl;
	
    float vertQuality = matchedDaughterWeight/recoDaughterWeight;
    
    bestTrackVertMatch[vertQuality] = std::make_pair(trackingVertex, simDaughterCounter);

      //! don't use this selection criteria for the moment, maybe get back to them later
      // Sanity condition in case that reconstructable condition is too tight
//     if ( simDaughterCounter < matchedDaughterCounter )
//       simDaughterCounter = matchedDaughterCounter;
//       
//       // Condition over S2RMatchedSimRatioi
//     if ( (double)matchedDaughterCounter/simDaughterCounter < 0.5 ) continue;	//! change value???
// 
//     double quality = (double)matchedDaughterWeight/recoDaughterWeight;
//   
//     // Condition over R2SMatchedRecoRatio
//     if (quality < 0.5) continue;	//! change value???

    /// take the first trackingVertex that meets criteria assuming that all other found trackingVertex are wrong anyway
    
  }
  
  outputRef = bestTrackVertMatch.rbegin()->second.first;
  quality = bestTrackVertMatch.rbegin()->first;
  numberDaughters = bestTrackVertMatch.rbegin()->second.second;
  
  if (debugMsg_)
    std::cout << "    Picked Tracking Vertex weight: " << bestTrackVertMatch.rbegin()->first << std::endl << std::endl;
  
  return std::make_pair(std::make_pair(quality,numberDaughters), outputRef);
  
}

int VertexClassifierWeight::getTrackingParticlePdgId(TrackingVertexRef const & trackingVertex) const
{
    if ( trackingVertex.isNonnull() )
    {
	
        //! don't use trails any longer, flag directly!
//         simVertexTrail_.push_back(trackingVertex);
	if ( !trackingVertex->genVertices().empty() )
        {	    
	    /// navigate over all the associated generated vertexes
	    int genVertPdgId = 0;
            for ( TrackingVertex::genv_iterator ivertex = trackingVertex->genVertices_begin(); ivertex != trackingVertex->genVertices_end(); ++ivertex )
	    {
		genVertPdgId = getGenParticlePdgId(const_cast<HepMC::GenVertex*>(&(**ivertex)));
		if (genVertPdgId)
		    return genVertPdgId;
	    }
            return 0;
        }
        else if ( !trackingVertex->sourceTracks().empty() )
        {

            // select the original source in case of combined vertices
            TrackingParticleRef const & sourceTrack = getSourceTrack(trackingVertex);
	    if (sourceTrack.isNull())
	    {
		return 0;
	    }
            return std::abs(sourceTrack->pdgId());
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

    return 0;
}

int VertexClassifierWeight::getGenParticlePdgId(HepMC::GenVertex  * genVertex) const
{
    if (genVertex)
    {
       
        // Verify if the vertex has incoming particles
        if ( genVertex->particles_in_size() )
	{
	    HepMC::GenParticle * genParticle = /*const_cast<HepMC::GenParticle*>(*/*(genVertex->particles_in_const_begin());
	    if ( !(genParticle->status() <= 2 && (genParticle->pdg_id() < 88 || genParticle->pdg_id() > 99)) )
	    {
		return 0;
	    }
	    double travelDistance; // in mm
	    HepMC::GenVertex * parentVertex = genParticle->production_vertex();
	    if (parentVertex)
		travelDistance = distanceToPrevVertex(genVertex, parentVertex);
	    else
		travelDistance = 0;
	    
	    if (travelDistance > 0.01)
	    {
		int pdgid = std::abs(genParticle->pdg_id());
		
		return pdgid;
	    }
	    else
	    {
		return getGenParticlePdgId(parentVertex);
	    }
	    
	}
	else
	{
	  if (debugMsg_)
	    std::cout << "    GenVertex has no incoming Particles." << std::endl << std::endl;
	  return 0;
	}
    }
    return 0;
}


