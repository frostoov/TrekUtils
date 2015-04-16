#ifndef CHAMBERMANAGER_HPP
#define CHAMBERMANAGER_HPP

#include <cstdint>
#include <unordered_map>
#include <array>

#include "tdcdata/structs.hpp"
#include "math/lines.hpp"
#include "math/coordsystem.hpp"

namespace trek {

using ChamberPoints = std::array<vecmath::Vec3, 3>;
/**
 * @class ChamberPosition
 * @author frostoov
 * @date 03/18/15
 * @file chamberconfigparser.hpp
 * @brief Описание положения дрейфовой камеры
 */
struct ChamberPosition {
	ChamberPoints points;  /*!< Точки дрейфовой камеры */
	unsigned int plane;	   /*!< Номер плоскости дрейфовой камеры */
	unsigned int group;	   /*!< Номер группы дрейфовой камеры */
};

using ChamberConfig = std::unordered_map<uintmax_t, ChamberPosition>;

/**
 * @class ChamberHandler
 * @author frostoov
 * @date 03/15/15
 * @file chambermanager.hpp
 * @brief Обработка mChamberTrackданны события дрейфовой камеры
 */
class ChamberHandler {
  public:

	using UIntVector	= std::vector<uint32_t>;
	using ChamberData	= std::array<UIntVector, 4>;
	using PointVector	= std::vector<vecmath::Vec2>;

	/**
	 * @class TrackDesc
	 * @author frostoov
	 * @date 03/15/15
	 * @file chambermanager.hpp
	 * @brief Структура с данными одного трека
	 */
	struct TrackDesc {
		vecmath::Line2					line;		/*!< Линия трека */
		PointVector				points;		/*!< Точки, по которым был восстановлен трек */
		double					dev;		/*!< Отклонение линии */
		std::array<uint32_t, 4>	times;		/*!< Вермена с TDC */
	};
	ChamberHandler(uint32_t pedestal = 0, double speed = 0);
	/**
	 * @brief Устанока данных события дрейфовой камеры и реконструкция трека
	 * @param chamberData Событие дрейфовой камеры
	 */

	void setChamberData(const ChamberData& chamberData);
	/**
	 * @brief Проверка наличия данных события с дрейфовых камер
	 * @return флаг наличия реконтрукции трека дрейфовых камер
	 */
	bool hasChamberData() const  {return mHasChamberData;}
	/**
	 * @brief Получение ссылки на событие с дрейфовых камер
	 */
	const ChamberData& getChamberEvent() const {
		if(mHasChamberData)
			return mChamberData;
		throw std::runtime_error("ChamberEventHandler: getChamberData: no chamber data");
	}
	/**
	 * @brief Проверка наличия реконструированного трека дрейфовых камер
	 * @return флаг наличия реконтрукции трека дрейфовых камер
	 */
	bool hasChamberTrack()	const { return mHasTrack; }
	/**
	 * @brief Получение ссылки на трек, восстановленного по данным дрейфовых камер
	 */
	const TrackDesc& getChamberTrack() const {
		if(mHasTrack)
			return mChamberTrack;
		throw std::runtime_error("ChamberEventHandler::getChamberTrack: no chamber track");
	}
	/**
	 * @brief Устанока пьедестала для данных с дрейфовой камеры
	 * @param pedestal значение пьедестала в [нс]
	 */
	void setPedestal(uint32_t pedestal) { mPedestal  = pedestal; }
	/**
	 * @brief Устанока скорости дрейфа
	 * @param speed скорость дрейфа электронов в [мм/нс]
	 */
	void setSpeed(double speed) { mSpeed = speed; }
  protected:
	void	createVariation(const ChamberData& chamData, const std::array<size_t, 4>& indices,
	                        size_t offset, std::array<uint32_t, 4>& variant);
	size_t	getDepth(const ChamberData& chamData);
	bool	createProjection();
	void    createProjection(TrackDesc& track);
	double	leastSquares(const PointVector& points, vecmath::Line2& line);
	bool	systemError(TrackDesc& track);
	double	getSystemError(double r, double ang) { return r*(1/std::cos(ang) - 1); }
  private:
	ChamberData mChamberData;
	TrackDesc	mChamberTrack;

	uint32_t	mPedestal;
	double		mSpeed;

	bool		mHasChamberData;
	bool		mHasTrack;

	const std::array<vecmath::Vec2, 4> mWires;
};

} //trek

#endif // CHAMBERMANAGER_HPP
