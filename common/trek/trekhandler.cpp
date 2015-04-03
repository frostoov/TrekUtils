#include <unordered_set>
#include "trekhandler.hpp"

namespace trek {

using std::vector;
using std::unordered_set;

void TrekHandler::loadEvent(const TUEvent& rawEvent)
{
	auto event = rawEvent.getTrekEvent();
	for(auto& chamber : mChambers ) {
		chamber.second.resetData();
		if(event.count(chamber.first)) {
			const auto& chamberEvent = event.at(chamber.first);
			mEventHandler.setChamberData(chamberEvent);
			if(mEventHandler.hasChamberTrack())
				chamber.second.setTrack(mEventHandler.getChamberTrack());
		}
	}
}

void TrekHandler::loadChambers(const ChamberConfig& chambers) {
	mChambers.clear();
	for(const auto& chamber : chambers)
		mChambers.insert({chamber.first, chamber.second});
}

void TrekHandler::createTrek() {
	mHasTrack = false;
	unordered_set<Chamber*> trackChambers;
	for(auto& chamber : mChambers) {
		trackChambers.insert(&chamber.second);
		if(trackChambers.size() == 2) {
			vector<Chamber*> chambers;
			chambers.assign(begin(trackChambers), end(trackChambers));
			if(chambers.at(0)->getGroup() == chambers.at(1)->getGroup() &&
			   chambers.at(0)->getPlane() != chambers.at(1)->getPlane()) {
				using vecmath::Plane;
				auto plane1 = chambers.at(0)->getTrackPlane();
				auto plane2 = chambers.at(1)->getTrackPlane();
				mTrack = Plane::getLine(plane1, plane2);
				mHasTrack = true;
			}
		}
	}

}

}
