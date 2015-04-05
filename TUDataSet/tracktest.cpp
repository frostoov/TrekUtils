#include <iostream>
#include "tracktest.hpp"

using namespace std;

TrackTest::TrackTest(const ChamberConfig& config, uint32_t pedestal, double speed)
	: mTrekHandler(config, pedestal, speed) {

}

TrackTest::~TrackTest()
{
}

void TrackTest::handleEvent(const TUEvent& event) {
	using vecmath::Line3;
	using vecmath::Vec3;
	mTrekHandler.loadEvent(event);
	mTrekHandler.createTrack();
	if(mTrekHandler.hasTrack()) {
		const auto& tTrack = mTrekHandler.getTTrack();
		const auto& uTrack = mTrekHandler.getUTrack();

		auto uVec = uTrack.vec().ort();
		auto tVec = tTrack.vec().ort();

		double angle = uVec.angle(tVec)*180./3.14;
		if(angle > 90)
			angle -= 90;

		cout << angle << std::endl;
	}

}
