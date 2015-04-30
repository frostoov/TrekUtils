#ifndef TUDATASET_STRUCTS_HPP
#define TUDATASET_STRUCTS_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <cstring>

#include "serialization.hpp"

namespace tdcdata {

using UIntVector	= std::vector<uint32_t>;

struct DateTime {
	uint8_t hsecond;	/**< сотые секунды */
	uint8_t second;		/**< секунда */
	uint8_t minute;		/**< минута */
	uint8_t hour;		/**< час */
	uint8_t day;		/**< день */
	uint8_t month;		/**< месяц */
	uint16_t year;		/**< год */

	void serialize(std::ostream& stream) const {
		::serialize(stream, hsecond);
		::serialize(stream, second);
		::serialize(stream, minute);
		::serialize(stream, hour);
		::serialize(stream, day);
		::serialize(stream, month);
		::serialize(stream, year);
	}
	void deserialize(std::istream& stream) {
		::deserialize(stream, hsecond);
		::deserialize(stream, second);
		::deserialize(stream, minute);
		::deserialize(stream, hour);
		::deserialize(stream, day);
		::deserialize(stream, month);
		::deserialize(stream, year);
	}
	static constexpr size_t getSize() {
		return ::getSize<decltype(hsecond)>() + ::getSize<decltype(second)>() +
		       ::getSize<decltype(minute)>() + ::getSize<decltype(hour)>() +
		       ::getSize<decltype(day)>() + ::getSize<decltype(month)>() +
		       ::getSize<decltype(year)>();
	}
};

struct NVDEvent {
	uint32_t	st;			/**< номер события */
	uint8_t		tr[2];		/**< статус биты[local,extern], маска сработавших плоскостей */
	uint16_t	ti[2];		/**< живое время время ожидания последнего события */
	uint32_t	dt[64];		/**< закодированные данные */

	void serialize(std::ostream& stream) const {
		::serialize(stream, st);
		::serialize(stream, tr, sizeof(tr));
		::serialize(stream, ti, sizeof(ti));
		::serialize(stream, dt, sizeof(dt));
	}
	void deserialize(std::istream& stream) {
		::deserialize(stream, st);
		::deserialize(stream, tr, sizeof(tr));
		::deserialize(stream, ti, sizeof(ti));
		::deserialize(stream, dt, sizeof(dt));
	}
	static constexpr size_t getSize() {
		return ::getSize<decltype(st)>() + sizeof(tr) + sizeof(ti) + sizeof(dt);
	}
};

struct UraganEvent { //Структура метки события для TSD
	int8_t		start[6];		/**< Ключевое слово начала записи */
	int16_t		type;			/**< Тип записи:0-Config,1-монитор,2-Experement event,3-Noise */
	uint32_t	nRun;			/**< Номер текущего рана */
	uint32_t	nEvent;			/**< Номер текущего события */
	DateTime	dt;				/**< Время события */
	int16_t		trackID;		/**< Признак наличия трека в URAGANе */
	double	chp0[3];			/**< Координаты точки трека в миллиметрах в системе НЕВОДа */
	double	chp1[3];			/**< Координаты точки трека в миллиметрах в системе НЕВОДа */
	NVDEvent	event;				/**<Кодированные данные события */
	void serialize(std::ostream& stream) const {
		::serialize(stream, start, sizeof(start));
		::serialize(stream, type);
		::serialize(stream, nRun);
		::serialize(stream, nEvent);
		::serialize(stream, dt);
		::serialize(stream, trackID);
		::serialize(stream, chp0, sizeof(chp0));
		::serialize(stream, chp1, sizeof(chp1));
		::serialize(stream, event);
	}
	void deserialize(std::istream& stream) {
		::deserialize(stream, start, sizeof(start));
		::deserialize(stream, type);
		::deserialize(stream, nRun);
		::deserialize(stream, nEvent);
		::deserialize(stream, dt);
		::deserialize(stream, trackID);
		::deserialize(stream, chp0, sizeof(chp0));
		::deserialize(stream, chp1, sizeof(chp1));
		::deserialize(stream, event);
	}
	static constexpr size_t getSize() {
		return sizeof(start) + ::getSize<decltype(type)>() + ::getSize<decltype(nRun)>() +
		       ::getSize<decltype(nEvent)>() + ::getSize<decltype(dt)>() +
		       ::getSize<decltype(trackID)>() +  sizeof(chp0) + sizeof(chp1) +
		       ::getSize<decltype(event)>();
	}
};

//Настройки TDC
struct TdcSettings {
	uint8_t		trigMode;	/**< Триггерный режим */
	uint8_t		subTrig;	/**< Trigger subtraction */
	uint8_t		tdcMeta;	/**< Дополнительные слова с TDC */
	uint16_t	winWidth;	/**< Ширина окна в нс */
	uint16_t	winOffset;	/**< Сдвиг окна в нс */
	uint16_t	detection;	/**< Тип измерения */
	uint16_t	lsb;		/**< Разрешение измерения */
	uint16_t	almostFull;	/**< Значение регистра almostFull */
	uint16_t	control;	/**< Значение регистра control */
	uint16_t	status;		/**< Значение регистра status */
	uint16_t	deadTime;	/**< Значение регистра deadTime */

	void serialize(std::ostream& stream) const {
		::serialize(stream, trigMode);
		::serialize(stream, subTrig);
		::serialize(stream, tdcMeta);
		::serialize(stream, winWidth);
		::serialize(stream, winOffset);
		::serialize(stream, detection);
		::serialize(stream, lsb);
		::serialize(stream, almostFull);
		::serialize(stream, control);
		::serialize(stream, status);
		::serialize(stream, deadTime);
	}
	void deserialize(std::istream& stream) {
		::deserialize(stream, trigMode);
		::deserialize(stream, subTrig);
		::deserialize(stream, tdcMeta);
		::deserialize(stream, winWidth);
		::deserialize(stream, winOffset);
		::deserialize(stream, detection);
		::deserialize(stream, lsb);
		::deserialize(stream, almostFull);
		::deserialize(stream, control);
		::deserialize(stream, status);
		::deserialize(stream, deadTime);
	}
	static constexpr size_t getSize() {
		return ::getSize<decltype(trigMode)>()  + ::getSize<decltype(subTrig)>() +
		       ::getSize<decltype(tdcMeta)>()   + ::getSize<decltype(winWidth)>() +
		       ::getSize<decltype(winOffset)>() + ::getSize<decltype(detection)>() +
		       ::getSize<decltype(lsb)>()       + ::getSize<decltype(almostFull)>() +
		       ::getSize<decltype(control)>()   + ::getSize<decltype(status)>() +
		       ::getSize<decltype(deadTime)>();
	}
};
struct TUDataSetHeader {
	//Возвращает значение коэфициента разрешеия измерения TDC
	double getLSBKoef() const {
		switch(settings.lsb) {
			case 0:
				return 0.781;
			case 1:
				return 0.195;
			case 2:
				return 0.098;
			default:
				throw std::runtime_error("Invalid valud of lsb koeficient");
		}
	}
	bool isValid() {
		if (key !=  52015 || fileType > 1)
			return false;
		return true;
	}
	uint16_t	fileType;	/**< Тип файла: 0 - без данныйх с Урагана, 1 - с даннными с урагана */
	uint16_t	key;		/**< Контрольное поле */
	uint64_t	fileSize;	/**< Размер файла в байтах */
	TdcSettings	settings;	/**< Настройки TDC, при которых осуществлялся набор */

	void serialize(std::ostream& stream) const {
		::serialize(stream, fileType);
		::serialize(stream, key);
		::serialize(stream, fileSize);
		::serialize(stream, settings);
	}
	void deserialize(std::istream& stream) {
		::deserialize(stream, fileType);
		::deserialize(stream, key);
		::deserialize(stream, fileSize);
		::deserialize(stream, settings);
	}

	static constexpr size_t getSize() {
		return ::getSize<decltype(fileType)>() + ::getSize<decltype(key)>() +
		       ::getSize<decltype(fileSize)>() + ::getSize<decltype(settings)>();
	}
};

} //tudata

#endif // TUDATASET_STRUCTS_HPP

