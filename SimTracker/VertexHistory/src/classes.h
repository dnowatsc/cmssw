#ifdef DICTGEN
#include "ivftree.hpp"
#else
#include "SimTracker/VertexHistory/interface/ivftree.hpp"
#endif

#include "Rtypes.h"

namespace {
 namespace {
    LorentzVector lv;
    GlobalVector v;
    ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<Double32_t>,ROOT::Math::DefaultCoordinateSystemTag> p;
    CovarianceMatrix cm;
    std::vector<double> f;
    ROOT::Math::MatRepSym<double,3> mrs;
    PV3DBase<float,VectorTag,GlobalTag> pvb;
    Basic3DVector<float> bv;
    ROOT::Math::RowOffsets<3> ros;
    
//     math::Error<dimension>::type e;
    
    TV tv;
 }
}
