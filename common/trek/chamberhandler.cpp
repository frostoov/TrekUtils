#include <limits>
#include <iostream>
#include "chamberhandler.hpp" ///


namespace trek {

using std::runtime_error;
using std::numeric_limits;

using CoordSystem3	= ChamberEventHandler::CoordSystem3;
using TrackDesc		= ChamberEventHandler::TrackDesc;
using Line2			= ChamberEventHandler::Line2;

template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

ChamberEventHandler::ChamberEventHandler(uint32_t pedestal, double speed)
	: mPedestal(pedestal), mSpeed(speed), mHasUraganTrack(false),
	  mHasChamberTrack(false), mHasChamberSystem(false),
	  mWires{ {Vec2{41., 0.75}, Vec2{51., -0.75}, Vec2{61., 0.75}, Vec2{71., -0.75}} }
{}

void ChamberEventHandler::setChamberData(const ChamberData& chamberData) {
	if(checkDepth(chamberData)) {
		for(size_t i = 0; i < mChamberTrack.times.size(); ++i) {
			if( chamberData[i].front() > mPedestal )
				mChamberTrack.times[i] = (chamberData[i].front() - mPedestal) * mSpeed;
			else {
				mHasChamberTrack = false;
				return;
			}
		}
		mHasChamberTrack = createProjection();
		if(mHasChamberTrack)
			for(size_t i = 0; i < mChamberTrack.times.size(); ++i)
				mChamberTrack.times[i] = chamberData[i].front();
	} else
		mHasChamberTrack = false;

}

void ChamberEventHandler::setUraganData(const UraganEvent& event) {
	if(mHasChamberSystem == true) {
		auto point1 = Vec3( event.chp0[0], event.chp0[1], event.chp0[2] );
		auto point2 = Vec3( event.chp1[0], event.chp1[1], event.chp1[2] );

		mChamberSystem.convertTo(point1);
		mChamberSystem.convertTo(point2);

		mUraganTrack = Line2( Vec2(point1.x(), point1.y()),
							  Vec2(point2.x(), point2.y()));
		if(!mHasUraganTrack)
			mHasUraganTrack = true;
	} else
		mHasUraganTrack = false;
}

const TrackDesc& ChamberEventHandler::getChamberTrack() const {
	if(mHasChamberTrack == false)
		throw runtime_error("No Chamber Track");
	return mChamberTrack;
}

const Line2& ChamberEventHandler::getUraganTrack() const {
	if(mHasUraganTrack == false)
		throw runtime_error("No Uragan Track");
	return mUraganTrack;
}

bool ChamberEventHandler::createProjection() {
	auto flag = createProjection(mChamberTrack);
	auto ret = flag != -1 ? true : false;
	return ret;
//	if(flag != -1)
//		return systemError(mChamberTrack);
//	else
//		return false;
}

size_t ChamberEventHandler::getDepth(const ChamberData& chamData) {
	auto depth = numeric_limits<size_t>::max();
	for(const auto& wireData : chamData)
		if(wireData.size() < depth)
			depth = wireData.size();
	return depth;
}

bool ChamberEventHandler::checkDepth(const ChamberData& chamData) {
	for(const auto& wireData : chamData)
		if(wireData.size() != 1)
			return false;
	return true;
}

bool ChamberEventHandler::systemError(ChamberEventHandler::TrackDesc& track) {
	double r;
	for(size_t i = 0; i < track.points.size(); ++i) {
		auto trackSign = sign(track.points[i].y());
		switch ( trackSign * sign(mWires[i].y()) ) {
			case 1:
				r = (std::abs(track.points[i].y()) > 6.2) ? 6.2 : track.points[i].y();
				break;
			case -1:
				r = (std::abs(track.points[i].y()) > 3.6) ? 3.6 : track.points[i].y();
				break;
			default:
				std::cout << "sys = " << std::endl;
				return false;
		}
		track.points[i].y() += trackSign*getSystemError(r, std::atan(track.line.k()) );
	}
	track.dev = leastSquares( track.points, track.line );
	return true;
}


void ChamberEventHandler::setChamberPosition(const ChamberPosition& pos) {
	setChamberPosition(getChamberSystem(pos));
}

void ChamberEventHandler::setChamberPosition(const CoordSystem3& system) {
	mChamberSystem = system;
	mHasChamberSystem = true;
}

CoordSystem3 ChamberEventHandler::getChamberSystem(const ChamberPosition& pos) {
	auto xAxis = Vec3( pos.vertices[1] - pos.vertices[0] ).ort();
	auto zAxis = Vec3( pos.vertices[2] - pos.vertices[0] ).ort();
	return CoordSystem3(pos.vertices[0], xAxis, zAxis);
}

void ChamberEventHandler::resetPosition() {
	mHasUraganTrack = false;
}

double ChamberEventHandler::createProjection(TrackDesc& track) {
	static PointVector tempPoints = {mWires[0], mWires[1], mWires[2], mWires[3]};
	static Line2 tempLine;
	size_t numPermutations = std::pow(2, track.times.size());
	track.points = tempPoints;
	track.dev = numeric_limits<double>::max();

	for(size_t i = 0; i < numPermutations; ++i) {
		//Изменяем знаки на противоположные
		for (size_t j = 0; j < track.times.size(); j++) {
			if( i & (1<<j) )
				tempPoints[j].y() = -static_cast<double>(track.times[j]);
			else
				tempPoints[j].y() =  static_cast<double>(track.times[j]);
			tempPoints[j].y() += mWires[j].y();
		}
		double tempDev = leastSquares(tempPoints,tempLine);
		if(tempDev != -1 && tempDev < track.dev) {
			track.dev = tempDev;
			for(int i = 0; i < 4; ++i)
				track.points[i].y() = tempPoints[i].y();
		}
	}
	track.dev = leastSquares(track.points, track.line);
	return track.dev;
}

double ChamberEventHandler::leastSquares(const PointVector& points, Line2& line) {
	if(points.size() < 2)
		return -1;
	double sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
	for(const auto& p : points) {
		sumX  += p.x();
		sumY  += p.y();
		sumXY += p.x() * p.y();
		sumXX += p.x() * p.x();
	}
	auto exp = points.size() * sumXX - sumX*sumX;
	if( exp && std::abs(exp) > 1e-60) {
		line.k() = (points.size()*sumXY - sumX*sumY)/exp;
		line.b() = (sumY - line.k()*sumX)/points.size();
		double dev = 0;
		for(const auto& p : points)
			dev += std::pow( (line.k() * p.x() + line.b()) - p.y(), 2);
		return dev;
	} else
		return -1;
}

} //trek
