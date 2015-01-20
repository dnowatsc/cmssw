// containers for mc truth information
#ifndef CategoryClasses_h
#define CategoryClasses_h

#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "SimTracker/TrackAssociation/interface/TrackAssociatorBase.h"


struct TrackMCInformation
{

	/*------QUALITY INFORMATION-----
	 * What to include here?
	 * - matching quality to TP (from quickTABH) --> see VertexCategorizer.cc, line 213
	 * - spatial pull wrt matched TP --> see SimTracker/TrackHistory/src/TrackClassifier.cc, line 156 ff.
	 * - information about BadInnerHits & SharedInnerHits --> see SimTracker/TrackHistory/src/TrackClassifier.cc, line 231 but apparantly causes segmentation violation
	     (see VertexCategorizationDOC.txt, line 44)
	 */



	/*-------OTHER INFORMATION-----
	 * e.g. error, 4-vector, mother vertex position/ref, ref to actual Candidate/Track/TrackingParticle
	 */

	 /*-------HISTORY INFORMATION-----
	 * e.g. using SimTracker/TrackHistory/interface/TrackHistory.h
	 */

	 edm::RefToBase<reco::Track> recoTrackRef;

	// with reco::RecoToSimCollection::data_type := std::pair<edm::Ref<reco::TrackingParticleCollection>, double>
	// std::vector<reco::RecoToSimCollection::data_type > tpRefWithQualityPairs;

	 double trackWeight;

	 int motherParticleID;

	 int geantProcessType;

	 int vertexType;

	// TrackQuality is about about the hits, i.e. whether some hits were missed for the reco, whether some of these hits are bad in the first place etc.
	// In a previous version, this was causing a segmentation violation (see VertexCategorizationDOC.txt) so don't use it for now; maybe this kind of information is to detailed anyway but could be useful
	// TrackQuality quality;

	// The associated double from the qTABH
	 double matchingQuality;
	// The actual spatial difference between reco and gen track
	 double dxyPull, dzPull;

};



struct VertexMCInformation
{
	edm::RefToBase<reco::VertexCompositePtrCandidate> vertexRef;

	reco::RecoToSimCollection trackRecoToSim;

	std::vector<TrackMCInformation> trackInfos;
};

#endif