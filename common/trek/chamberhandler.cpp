#include <limits>
#include "chamberhandler.hpp"


namespace trek {

using std::array;
using std::runtime_error;
using std::numeric_limits;

using vecmath::Vec2;
using vecmath::Line2;

template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

ChamberHandler::ChamberHandler(uint32_t pedestal, double speed)
	: mPedestal(pedestal), mSpeed(speed), mHasChamberData(false), mHasTrack(false),
	  mWires{ {Vec2{41., 0.75}, Vec2{51., -0.75}, Vec2{61., 0.75}, Vec2{71., -0.75}} }
{ }

void ChamberHandler::setChamberData(const Event& chamberEvent) {
	for(auto wireData : mChamberDistances)
		wireData.clear();
	mHasChamberData = false;
	if(getDepth(chamberEvent) == 1)
		mHasTrack = createProjection(chamberEvent);
	else
		mHasTrack = false;
}

void ChamberHandler::getDistances(const Event& data, Distances& distances) {
	for(size_t i = 0; i < data.size(); ++i) {
		distances.at(i).clear();
		for(auto msr :  data.at(i))
			if(msr > mPedestal)
				distances.at(i).push_back((msr - mPedestal)*mSpeed);
	}
}

bool ChamberHandler::createProjection(const Event& chamberEvent) {
	getDistances(chamberEvent, mChamberDistances);
	mHasChamberData = true;
	Points tmpPoints;
	Line2		tmpLine;
	array<uint32_t, 4> distances;
	mChamberTrack.dev = numeric_limits<double>::infinity();
	array<uintmax_t, 4> ind;
	for(ind.at(0) = 0; ind.at(0) != mChamberDistances.at(0).size(); ++ind.at(0))
		for(ind.at(1) = 0; ind.at(1) != mChamberDistances.at(1).size(); ++ind.at(1))
			for(ind.at(2) = 0; ind.at(2) != mChamberDistances.at(2).size(); ++ind.at(2))
				for(ind.at(3) = 0; ind.at(3) != mChamberDistances.at(3).size(); ++ind.at(3)) {
					createVariation(mChamberDistances,ind, distances);
					auto dev = createProjection(distances, tmpPoints, tmpLine);
					if(dev != -1 && dev < mChamberTrack.dev) {
						mChamberTrack.dev    = dev;
						mChamberTrack.points = tmpPoints;
						mChamberTrack.line   = tmpLine;
						createVariation(chamberEvent, ind, mChamberTrack.times);
					}
				}
	if(mChamberTrack.dev < numeric_limits<double>::infinity())
		return systemError(mChamberTrack);
	else
		return false;
}

size_t ChamberHandler::getDepth(const Event& chamData) {
	auto depth = numeric_limits<size_t>::max();
	for(const auto& wireData : chamData)
		if(wireData.size() < depth)
			depth = wireData.size();
	return depth;
}

bool ChamberHandler::systemError(ChamberHandler::TrackDesc& track) {
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

template<typename Source, typename Dest>
void ChamberHandler::createVariation(const Source& chamData,  const std::array<uintmax_t, 4>& indices,
                                     Dest& variant) {
	for(size_t i = 0; i < variant.size(); ++i)
		variant.at(i) = chamData.at(i).at(indices.at(i)%chamData.at(i).size());
}

double ChamberHandler::createProjection(const array<uint32_t, 4>& distances,
                                        Points& points, Line2& line) {
	static Points tempPoints = {mWires[0], mWires[1], mWires[2], mWires[3]};
	static Line2 tempLine;
	auto dev = numeric_limits<double>::infinity();
	size_t numPermutations = std::pow(2, distances.size());


	for(size_t i = 0; i < numPermutations; ++i) {
		//Изменяем знаки на противоположные
		for (size_t j = 0; j < distances.size(); j++) {
			if( i & (1<<j) )
				tempPoints[j].y() = -static_cast<double>(distances[j]);
			else
				tempPoints[j].y() =  static_cast<double>(distances[j]);
			tempPoints[j].y() += mWires[j].y();
		}
		double tempDev = leastSquares(tempPoints,tempLine);
		if(tempDev != -1 && tempDev < dev) {
			dev = tempDev;
			line = tempLine;
			points = tempPoints;
		}
	}
	return dev;
}

double ChamberHandler::leastSquares(const Points& points, Line2& line) {
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
