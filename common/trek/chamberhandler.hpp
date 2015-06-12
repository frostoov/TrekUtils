#ifndef CHAMBERMANAGER_HPP
#define CHAMBERMANAGER_HPP

#include <cstdint>
#include <unordered_map>
#include <array>

#include "trektypes.hpp"
#include "tdcdata/tdcdatatypes.hpp"
#include "math/lines.hpp"
#include "math/coordsystem.hpp"

namespace trek {

class Chamber;


/**
 * @class ChamberHandler
 * @author frostoov
 * @date 03/15/15
 * @file chambermanager.hpp
 * @brief Обработка mChamberTrackданны события дрейфовой камеры
 */
class ChamberHandler {
	using Indecies      = std::array<uintmax_t, 4>;
  public:
	static TrackDescription createTrackDescription(const ChamberTimes& eventTimes, const ChamberDescription& chamDesc);
  protected:
	static size_t           getDepth(const ChamberDistances& eventDistances);
	static ChamberDistances getDistances(const ChamberTimes& data, const ChamberDescription& chamDesc);
	static TrackDescription createTrackDescription(const TrackDistances& distances);
	static TrackDistances   createTrackDistances(const ChamberDistances& eventDistances, const Indecies& indices);
	static TrackTimes       createTrackTimes(const ChamberTimes& eventTimes, const Indecies& indices);
	static double	leastSquares(const Points& points, vecmath::Line2& line);
	static bool	    systemError(TrackDescription& track);
	static double	getSystemError(double r, double ang);
  private:
	static const std::array<vecmath::Vec2, 4> mWires;
};

} //trek

#endif // CHAMBERMANAGER_HPP
