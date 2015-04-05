#include <limits>
#include "chamberhandler.hpp"


namespace trek {

using std::array;
using std::runtime_error;
using std::numeric_limits;

using CoordSystem3	= ChamberEventHandler::CoordSystem3;
using TrackDesc		= ChamberEventHandler::TrackDesc;
using Line2			= ChamberEventHandler::Line2;

template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

ChamberEventHandler::ChamberEventHandler(uint32_t pedestal, double speed)
	: mPedestal(pedestal), mSpeed(speed), mHasChamberData(false), mHasTrack(false),
	  mWires{ {Vec2{41., 0.75}, Vec2{51., -0.75}, Vec2{61., 0.75}, Vec2{71., -0.75}} }
{}

void ChamberEventHandler::setChamberData(const ChamberData& chamberData) {
	for(size_t i = 0; i < chamberData.size(); ++i) {
		mChamberData.at(i).clear();
		for(auto msr :  chamberData.at(i))
			if(msr > mPedestal)
				mChamberData.at(i).push_back((msr - mPedestal)*mSpeed);
	}
	mHasChamberData = true;
	if(getDepth(chamberData) == 1)
		mHasTrack = createProjection();
	else
		mHasTrack = false;
}

//void ChamberEventHandler::setUraganData(const Line3& line) {
//	Line3 tmpLine = line;
//	mChamberSystem.convertTo(tmpLine.dot());
//	mChamberSystem.convertTo(tmpLine.vec());

//	auto point1 = tmpLine.dot() + tmpLine.vec()*5;

//	mUraganTrack = Line2(Vec2(tmpLine.dot().x(), tmpLine.dot().y()),
//						 Vec2(point1.x(), point1.y()));
//	if(!mHasUraganTrack)
//		mHasUraganTrack = true;
//}

//void ChamberEventHandler::setUraganData(const UraganEvent& event) {
//	if(mHasChamberSystem == true) {
//		auto point1 = Vec3( event.chp0[0], event.chp0[1], event.chp0[2] );
//		auto point2 = Vec3( event.chp1[0], event.chp1[1], event.chp1[2] );

//		mChamberSystem.convertTo(point1);
//		mChamberSystem.convertTo(point2);

//		mUraganTrack = Line2( Vec2(point1.x(), point1.y()),
//							  Vec2(point2.x(), point2.y()));
//		if(!mHasUraganTrack)
//			mHasUraganTrack = true;
//	} else
//		mHasUraganTrack = false;
//}

bool ChamberEventHandler::createProjection() {
	if(!mHasChamberData)
		throw std::runtime_error("ChamberEventHandler::createProjection: no chamber data");
	TrackDesc tmpTrack;
	mChamberTrack.dev = numeric_limits<double>::infinity();
	array<size_t, 4> ind;
	for(ind.at(0) = 0; ind.at(0) != mChamberData.at(0).size(); ++ind.at(0))
		for(ind.at(1) = 0; ind.at(1) != mChamberData.at(1).size(); ++ind.at(1))
			for(ind.at(2) = 0; ind.at(2) != mChamberData.at(2).size(); ++ind.at(2))
				for(ind.at(3) = 0; ind.at(3) != mChamberData.at(3).size(); ++ind.at(3)) {
					createVariation(mChamberData,ind, 0, tmpTrack.times);
					createProjection(tmpTrack);
					if(tmpTrack.dev != -1 && tmpTrack.dev < mChamberTrack.dev)
						mChamberTrack = tmpTrack;
				}
	if(mChamberTrack.dev < numeric_limits<double>::infinity()) {
		for(auto& time : mChamberTrack.times)
			time = time/mSpeed + mPedestal;
		return systemError(mChamberTrack);
	} else
		return false;
}

size_t ChamberEventHandler::getDepth(const ChamberData& chamData) {
	auto depth = numeric_limits<size_t>::max();
	for(const auto& wireData : chamData)
		if(wireData.size() < depth)
			depth = wireData.size();
	return depth;
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
				return false;
		}
		track.points[i].y() += trackSign*getSystemError(r, std::atan(track.line.k()) );
	}
	track.dev = leastSquares( track.points, track.line );
	return true;
}

//void ChamberEventHandler::setChamberPosition(const array<Vec3, 3>& pos) {
//	setChamberPosition(getChamberSystem(pos));
//}

//void ChamberEventHandler::setChamberPosition(const CoordSystem3& system) {
//	mChamberSystem = system;
//	mHasChamberSystem = true;
//}



void ChamberEventHandler::createVariation(const ChamberData& chamData,  const array<size_t, 4>& indices,
										  size_t offset, std::array<uint32_t, 4>& variant) {
	for(size_t i = 0; i < variant.size(); ++i)
		variant.at(i) = chamData.at(i).at((indices.at(i) + offset)%chamData.at(i).size());
}

void ChamberEventHandler::createProjection(TrackDesc& track) {
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
