#include <unordered_set>
#include <iostream>
#include "trekhandler.hpp"

namespace trek {

using std::begin;
using std::end;
using std::array;
using std::vector;
using std::unordered_set;

TrekHandler::TrekHandler(const ChamberConfig& config, uint32_t pedestal, double speed)
	: mEventHandler(pedestal, speed), mHasTrack(false) {
	loadChambers(config);
}

void TrekHandler::loadEvent(const TUEvent& rawEvent) {
	auto& uEvent = rawEvent.getUraganEvent();
	mUTrack = Line3(Vec3(uEvent.chp0[0], uEvent.chp0[1], uEvent.chp0[2]),
	                Vec3(uEvent.chp1[0], uEvent.chp1[1], uEvent.chp1[2]));
	auto event = rawEvent.getTrekEvent();
	for(auto& chamber : mChambers ) {
		chamber.second.resetData();
		if(event.count(chamber.first)) {
			const auto& chamberEvent = event.at(chamber.first);
			mEventHandler.setChamberData(chamberEvent);
			chamber.second.setTrack(mEventHandler);
		}
	}
}

void TrekHandler::loadChambers(const ChamberConfig& chambers) {
	mChambers.clear();
	for(const auto& chamber : chambers)
		mChambers.insert({chamber.first, chamber.second});
}

void TrekHandler::createTrack() {
	mHasTrack = false;
	unordered_set<const Chamber*> trackChambers;
	for(const auto& chamber : mChambers) {
		if(chamber.second.hasTrack())
			trackChambers.insert(&chamber.second);
	}
	if(trackChambers.size() == 2) {
		vector<const Chamber*> chambers;
		chambers.assign(begin(trackChambers), end(trackChambers));
		if(chambers.at(0)->getGroup() == chambers.at(1)->getGroup() &&
		        chambers.at(0)->getPlane() != chambers.at(1)->getPlane()) {
			mTTrack = createTrack(*chambers.at(0), *chambers.at(1));
			mHasTrack = true;
		}
	}
}

TrekHandler::Line3 TrekHandler::createTrack(const Chamber& cham1, const Chamber& cham2) {
	if(cham1.getGroup() != cham2.getGroup() || cham1.getPlane() == cham2.getPlane())
		throw std::runtime_error("TrekHandler::createTrek: invalid chambers");
	return createTrack(cham1.getTrack(), cham1.getPoints(),
	                   cham2.getTrack(), cham2.getPoints());
}

TrekHandler::Line3 TrekHandler::createTrack(const Line2& track1, const ChamberPoints& pos1,
        const Line2& track2, const ChamberPoints& pos2) {
	const auto plane1 = Chamber::getTrackPlane(track1, pos1);
	const auto plane2 = Chamber::getTrackPlane(track2, pos2);

	return vecmath::Plane::getLine(plane1, plane2);
}

void TrekHandler::loadVertices(std::vector<float>& data) const {
	if(mHasTrack) {
		double delta = 10900 - mTTrack.dot().z();
		auto vec = mTTrack.vec().ort();
		vec = vec/vec.z();
		auto dot = mTTrack.dot() + vec*delta;

		std::cout << dot.z() << std::endl;

		Vec3 point1(dot + mTTrack.vec().ort()*10000);
		Vec3 point2(dot - mTTrack.vec().ort()*10000);
		data.insert(end(data), begin(point1), end(point1));
		data.insert(end(data), begin(point2), end(point2));
	}
	double delta = 10900 - mUTrack.dot().z();
	auto vec = mUTrack.vec().ort();
	vec = vec/vec.z();
	auto dot = mUTrack.dot() + vec*delta;

	std::cout << dot.z() << std::endl;

	Vec3 point1(dot + mUTrack.vec()*10000);
	Vec3 point2(dot - mUTrack.vec()*10000);
	data.insert(end(data), begin(point1), end(point1));
	data.insert(end(data), begin(point2), end(point2));
}

void TrekHandler::loadLineColors(std::vector<float>& data) const {
	if(mHasTrack) {
		static float trackColor[] = {
			0,1,0,1,
			0,1,0,1,
		};
		data.insert(end(data), std::begin(trackColor), std::end(trackColor));
	}
	static float trackColor[] = {
		0,0,1,1,
		0,0,1,1,
	};
	data.insert(end(data), std::begin(trackColor), std::end(trackColor));
}

void TrekHandler::loadLineFace(std::vector<unsigned>& face, unsigned start) const {
	face.push_back(start);
	face.push_back(start + 1);
	if(mHasTrack) {
		face.push_back(start + 2);
		face.push_back(start + 3);
	}
}

} //vecmath
