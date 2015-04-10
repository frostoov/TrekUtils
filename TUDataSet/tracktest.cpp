//#include <iostream>
//#include "tracktest.hpp"

//using namespace std;

//static constexpr double PI    = 3.14159265358979323846;
//static constexpr double todeg = 180./PI;

//TrackTest::TrackTest(const ChamberConfig& config, uint32_t pedestal, double speed)
//	: mTrekHandler(config, pedestal, speed) {

//}

//TrackTest::~TrackTest()
//{
//}

//void TrackTest::handleEvent(const TUEvent& event) {
//	using vecmath::Line3;
//	using vecmath::Vec3;
//	mTrekHandler.loadEvent(event);
//	mTrekHandler.createTrack();
//	if(mTrekHandler.hasTrack()) {
//		const auto& tTrack = mTrekHandler.getTTrack();
//		const auto& uTrack = mTrekHandler.getUTrack();

//		auto uVec = uTrack.vec().ort();
//		auto tVec = tTrack.vec().ort();

//		double angle = uVec.angle(tVec)*todeg;
//		if(angle > 90)
//			angle = 180 - angle;

//		cout << angle << std::endl;
//	}

//}
