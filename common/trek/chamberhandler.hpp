#ifndef CHAMBERMANAGER_HPP
#define CHAMBERMANAGER_HPP

#include <cstdint>
#include <array>

#include "tdcdata/structs.hpp"
#include "configparser/chamberconfigparser.hpp"
#include "math/lines.hpp"
#include "math/coordsystem.hpp"
#include "math/typedefs.hpp"

namespace trek {

/**
 * @class ChamberEventHandler
 * @author frostoov
 * @date 03/15/15
 * @file chambermanager.hpp
 * @brief Обработка данны события дрейфовой камеры
 */
class ChamberEventHandler {
  public:
	using Line2			= vecmath::Line2;
	using Vec2			= vecmath::Vec2;
	using Vec3			= vecmath::Vec3;
	using CoordSystem3	= vecmath::CoordSystem3;

	using UIntVector	= std::vector<uint32_t>;
	using ChamberData	= std::array<UIntVector, 4>;
	using TrackTimes	= std::vector< std::array<uint32_t,4> >;
	using LineVector	= std::vector<Line2>;
	using DoubleVector	= std::vector<double>;
	using PointVector	= std::vector<Vec2>;
	using SizeVector	= std::vector<size_t>;
	using UraganEvent   = tdcdata::UraganEvent;

	/**
	 * @class TrackDesc
	 * @author frostoov
	 * @date 03/15/15
	 * @file chambermanager.hpp
	 * @brief Структура с данными одного трека
	 */
	struct TrackDesc {
		Line2					line;		/*!< Линия трека */
		PointVector				points;		/*!< Точки, по которым был восстановлен трек */
		double					dev;		/*!< Отклонение линии */
		std::array<uint32_t, 4>	times;		/*!< Вермена с TDC */
	};
	ChamberEventHandler(uint32_t pedestal = 0, double speed = 0);
	/**
	 * @brief Устанока данных события дрейфовой камеры и реконструкция трека
	 * @param chamberData Событие дрейфовой камеры
	 */
	void setChamberData(const ChamberData& chamberData);
	/**
	 * @brief Устанока данных события с УРАГАНа
	 * @param event Событие с УРАГАНа
	 */
	void setUraganData(const UraganEvent& event);
	/**
	 * @brief Получение ссылки на трек, восстановленного по данным дрейфовых камер
	 */
	const TrackDesc& getChamberTrack() const;
	/**
	 * @brief Получение ссылки на трек, восстановленного по данным c УРАГАНа
	 */
	const Line2& getUraganTrack() const;
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
	/**
	 * @brief Устанока положения дрейфовой камеры
	 * @param pos Положения камеры в глобальных координатах НЕВОДа
	 */
	void setChamberPosition(const ChamberPosition& pos);
	void setChamberPosition(const CoordSystem3& system);
	static CoordSystem3 getChamberSystem(const ChamberPosition& pos);
	/**
	 * @brief Сброс положения дрейфовой камеры
	 */
	void resetPosition();
	/**
	 * @brief Проверка наличия реконструированного трека УРАГАНа
	 * @return флаг наличия реконтрукции трека УРАГАНа
	 */
	bool hasUraganTrack() const { return mHasUraganTrack; }
	/**
	 * @brief Проверка наличия реконструированного трека дрейфовых камер
	 * @return флаг наличия реконтрукции трека дрейфовых камер
	 */
	bool hasChamberTrack()	const { return mHasChamberTrack; }
	/**
	 * @brief Проверяет, задана ли система координат дрейфовой камеры в системе координа НЕВОД
	 * @return флаг наличия системы координат дрейфовой камеры
	 */
	bool hasChamberSystem() const { return mHasChamberSystem; }
  protected:
	size_t	getDepth(const ChamberData& mChamData);
	bool	createProjection();
	double	createProjection(TrackDesc& track);
	double	leastSquares(const PointVector& points, Line2& line);
	bool	checkDepth(const ChamberData& chamData);
	bool	systemError(TrackDesc& track);
	double	getSystemError(double r, double ang) { return r*(1/std::cos(ang) - 1); }
  private:
	TrackDesc	mChamberTrack;
	Line2		mUraganTrack;

	uint32_t	mPedestal;
	double		mSpeed;

	bool		mHasUraganTrack;
	bool		mHasChamberTrack;
	bool		mHasChamberSystem;

	const std::array<Vec2, 4> mWires;
	CoordSystem3	mChamberSystem;
};

} //trek

#endif // CHAMBERMANAGER_HPP
