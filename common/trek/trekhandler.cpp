#include <unordered_set>
#include <iostream>
#include "trekhandler.hpp"

namespace trek {

using std::begin;
using std::end;
using std::array;
using std::vector;
using std::unordered_set;

TrekHandler::TrekHandler(const ChamberConfig& config)
	: mHasTrack(false) {
	loadChambers(config);
}

void TrekHandler::loadEvent(const TUEvent& rawEvent) {
	const auto& uraganEvent = rawEvent.getUraganEvent();
	mUTrack = {{uraganEvent.chp0[0], uraganEvent.chp0[1], uraganEvent.chp0[2]},
		{uraganEvent.chp1[0], uraganEvent.chp1[1], uraganEvent.chp1[2]}
	};
	auto trekEvent = rawEvent.getTrekEvent();
	for(auto& chamberPair : mChambers ) {
		const auto chamberNumber = chamberPair.first;
		auto& chamber = chamberPair.second;
		if(trekEvent.count(chamberNumber)) {
			const auto& chamberEvent = trekEvent.at(chamberNumber);
			chamber.createTrack(chamberEvent);
		} else
			chamber.resetData();
	}
}

void TrekHandler::loadChambers(const ChamberConfig& chambers) {
	mChambers.clear();
	for(const auto& chamberDescPair : chambers)
		mChambers.insert({chamberDescPair.first, chamberDescPair.second});
}

bool TrekHandler::createTrack() {
	mHasTrack = false;
	unordered_set<const Chamber*> trackChambers;
	for(const auto& chamber : mChambers) {
		if(chamber.second.hasTrack())
			trackChambers.insert(&chamber.second);
	}
	if(trackChambers.size() == 2) {
		vector<const Chamber*> chambers;
		chambers.assign(begin(trackChambers), end(trackChambers));
		if(chambers.at(0)->getChamberGroup() == chambers.at(1)->getChamberGroup() &&
		        chambers.at(0)->getChamberPlane() != chambers.at(1)->getChamberPlane()) {
			mTTrack = createTrack(*chambers.at(0), *chambers.at(1));
			mHasTrack = true;
		}
	}
	return mHasTrack;
}

TrekHandler::Line3 TrekHandler::createTrack(const Chamber& cham1, const Chamber& cham2) {
	if(cham1.getChamberGroup() == cham2.getChamberGroup() && cham1.getChamberPlane() != cham2.getChamberPlane())
		return vecmath::Plane::getLine(cham1.getTrackPlane(), cham2.getTrackPlane());
	else
		throw std::runtime_error("TrekHandler::createTrek: invalid chambers");
}

} //vecmath
