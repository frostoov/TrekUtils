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

const std::array<vecmath::Vec2, 4> ChamberHandler::mWires{{
		Vec2(41,  0.75),
		Vec2(51, -0.75),
		Vec2(61,  0.75),
		Vec2(71, -0.75),
	}};

TrackDescription ChamberHandler::createTrackDescription(const ChamberTimes& eventTimes,
		const ChamberDescription& chamDesc) {
	ChamberDistances eventDistances(getDistances(eventTimes, chamDesc));
	if(getDepth(eventDistances) == 1) {
		TrackDescription trackDesc;
		trackDesc.deviation = numeric_limits<double>::infinity();
		Indecies ind;
		for(ind.at(0) = 0; ind.at(0) < eventDistances.at(0).size(); ++ind.at(0))
			for(ind.at(1) = 0; ind.at(1) < eventDistances.at(1).size(); ++ind.at(1))
				for(ind.at(2) = 0; ind.at(2) < eventDistances.at(2).size(); ++ind.at(2))
					for(ind.at(3) = 0; ind.at(3) < eventDistances.at(3).size(); ++ind.at(3)) {
						auto tempTrackDesc  = createTrackDescription( createTrackDistances(eventDistances, ind) );
						if(tempTrackDesc.deviation != -1 && tempTrackDesc.deviation < trackDesc.deviation) {
							tempTrackDesc.times = createTrackTimes(eventTimes, ind);
							trackDesc = tempTrackDesc;
						}
					}
		if(trackDesc.deviation != numeric_limits<double>::infinity() && systemError(trackDesc))
			return trackDesc;
		else
			throw runtime_error("ChamberHandler: createTrackDescription: cannot create track");
	} else
		throw runtime_error("ChamberHandler: createTrackDescription: cannot create track");
}

ChamberDistances ChamberHandler::getDistances(const ChamberTimes& eventTimes,
		const ChamberDescription& chamDesc) {
	ChamberDistances distances;
	for(size_t wire = 0; wire < eventTimes.size(); ++wire)
		for(auto msr :  eventTimes.at(wire)) {
			const auto& params = chamDesc.getParameters().at(wire);
			if(msr > params.getOffset())
				distances.at(wire).push_back((msr - params.getOffset())*params.getSpeed());
		}
	return distances;
}

size_t ChamberHandler::getDepth(const ChamberDistances& eventDistances) {
	auto depth = numeric_limits<size_t>::max();
	for(const auto& wireData : eventDistances)
		if(wireData.size() < depth)
			depth = wireData.size();
	return depth;
}

bool ChamberHandler::systemError(TrackDescription& track) {
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
		track.points[i].y() += trackSign * getSystemError(r, std::atan(track.line.k()) );
	}
	track.deviation = leastSquares( track.points, track.line );
	return true;
}

double ChamberHandler::getSystemError(double r, double ang) {
	return r * (1 / std::cos(ang) - 1);
}

TrackDistances ChamberHandler::createTrackDistances(const ChamberDistances& eventDistances,
		const Indecies& indices) {
	TrackDistances trackDistances;
	for(size_t i = 0; i < trackDistances.size(); ++i)
		trackDistances.at(i) = eventDistances.at(i).at(indices.at(i) % eventDistances.at(i).size());
	return trackDistances;
}

TrackTimes ChamberHandler::createTrackTimes(const ChamberTimes& eventTimes,
		const Indecies& indices) {
	TrackTimes trackTimes;
	for(size_t i = 0; i < trackTimes.size(); ++i)
		trackTimes.at(i) = eventTimes.at(i).at(indices.at(i) % eventTimes.at(i).size());
	return trackTimes;
}

TrackDescription ChamberHandler::createTrackDescription(const TrackDistances& distances) {
	TrackDescription trackDesc;
	static Points tempPoints{mWires[0], mWires[1], mWires[2], mWires[3]};
	static Line2  tempLine;
	trackDesc.deviation = numeric_limits<double>::infinity();
	size_t numPermutations = std::pow(2, distances.size());

	for(size_t i = 0; i < numPermutations; ++i) {
		//Изменяем знаки на противоположные
		for (size_t j = 0; j < distances.size(); j++) {
			if( i & (1 << j) )
				tempPoints[j].y() = -static_cast<double>(distances[j]);
			else
				tempPoints[j].y() =  static_cast<double>(distances[j]);
			tempPoints[j].y() += mWires[j].y();
		}
		double tempDev = leastSquares(tempPoints, tempLine);
		if(tempDev != -1 && tempDev < trackDesc.deviation) {
			trackDesc.deviation = tempDev;
			trackDesc.line = tempLine;
			trackDesc.points = tempPoints;
		}
	}
	return trackDesc;
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
	auto exp = points.size() * sumXX - sumX * sumX;
	if( exp && std::abs(exp) > 1e-60) {
		line.k() = (points.size() * sumXY - sumX * sumY) / exp;
		line.b() = (sumY - line.k() * sumX) / points.size();
		double dev = 0;
		for(const auto& p : points)
			dev += std::pow( (line.k() * p.x() + line.b()) - p.y(), 2);
		return dev;
	} else
		return -1;
}

} //trek
