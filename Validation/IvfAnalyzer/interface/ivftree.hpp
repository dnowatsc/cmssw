#ifndef VALIDATION_IVFANALYZER_IVFTREE_HPP
#define VALIDATION_IVFANALYZER_IVFTREE_HPP


// #include "DataFormats/Candidate/interface/Candidate.h"
// #include "Validation/IvfAnalyzer/interface/VertexClassifierWeight.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
// #include "DataFormats/VertexReco/interface/Vertex.h"
#include <vector>
#include "Math/Vector3D.h"
#include "Math/LorentzVector.h"
#include "Math/PositionVector3D.h"
#include "Math/SMatrix.h"

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVector; //ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >
// typedef ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<double> > Vector; // ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<double> >
typedef ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<double> > Point; // ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<double> >
typedef std::vector<double> Flags;
typedef ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double, 3> > CovarianceMatrix; // ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double, 3> >

struct TV
{
    Point tvPos;
    int pdgIdMother;
    LorentzVector p4daughters;
    int numberChargedDaughters;
//     LorentzVector p4mother; => NEED THIS?
};

struct recoVertex
{
    LorentzVector p4daughters;
    Point recoPos;
    
    int numberTracks;
    
    float impact2D, impactSig2D, impact3D, impactSig3D;
    
    GlobalVector flightdir;
    
    Flags thisProcessFlags;
    Flags historyProcessFlags;
    
    CovarianceMatrix error;
    
    float ndof;
    float chi2;
    
    TV trackingVertex;
    float tvquality;
    
};

#endif
