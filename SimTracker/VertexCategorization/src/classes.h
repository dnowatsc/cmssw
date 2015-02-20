#include "DataFormats/Common/interface/Wrapper.h"
#include "SimTracker/VertexCategorization/interface/CategoryClasses.h"
#include "DataFormats/Common/interface/View.h"

namespace
{
struct dictionary
{
// 	reco::RecoToSimCollection rtsc;
// 	edm::Wrapper<reco::RecoToSimCollection> wrtsc;

    std::vector<TrackMCInformation> dummy01;
    std::vector<VertexMCInformation> dummy02;
    edm::Wrapper<std::vector<TrackMCInformation> > dummy03;
    edm::Wrapper<std::vector<VertexMCInformation> > dummy04;
};
}

