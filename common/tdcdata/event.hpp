#ifndef TDSEVENT_HPP
#define TDSEVENT_HPP
#include <stdexcept>
#include <array>
#include <unordered_map>
#include <set>

#include "structs.hpp"

namespace tdcdata {

enum class EventType {
	simple,
	uragan,
};

/**
 * @class TEvent
 * @author frostoov
 * @date 03/29/15
 * @file tuevent.hpp
 * @brief Событие TDC
 */
class TDCEvent {
	friend class DataSet;
  public:
	using ChamberEvent	= std::array<UIntVector, 4>;
	using TrekEvent		= std::unordered_map<size_t,ChamberEvent>;
  public:
	TDCEvent() : mType(EventType::simple) {}
	/**
	 * @brief Возвращает время измерения
	 * @param word Слово TDC
	 * @return Время в [нс]
	 */
	static uint32_t getMsr (uint32_t word) { return (word & 0x7ffff); }
	/**
	 * @brief Возвращает номер камеры
	 * @param word Слово TDC
	 * @return Номер камеры
	 */
	static uint32_t getChamber (uint32_t word)	{ return ((word>>19) & 31); }
	/**
	 * @brief Возвращает номер проволки
	 * @param word Слово TDC
	 * @return Номер проволки
	 */
	static uint32_t getWire (uint32_t word)	{ return ((word>>24) & 3); }
	/**
	 * @brief Получение сырых данных с TDC
	 * @return Ссылка на константный вектор со словами TDC - std::vector<uint32_t>
	 */
	const UIntVector& getTrekEventRaw() const { return mTrekEvent; }
	/**
	 * @brief Создание и получение ассоциативного массива с измерениями со всей установки.
	 * @return Ассоциативный массив(std::unordered_map) с данными камер
	 */
	TrekEvent getTrekEvent()		const;
	/**
	 * @brief Получение ассоциативного массива с измерениями со всей установки
	 * @param trekEvent Ассоциативный массив куда будут загружены измерения
	 */
	void loadTrekEvent(TrekEvent& trekEvent) const;
	/**
	 * @brief Создание и получение массива с измерениями с камеры
	 * @param cham Номер камеры
	 */
	ChamberEvent getChamberEvent(size_t cham) const;
	/**
	 * @brief Получение массива с измерениями с камеры
	 * @param chamEvent Массив куда будут загружены измерения
	 * @param cham Номер камеры
	 */
	void loadChamberEvent(ChamberEvent& chamEvent, size_t cham) const;
	/**
	 * @brief Создать и получить множество номеров сработавших камер
	 * @return Множество номеров сработавших камер
	 */
	std::set<uintmax_t> getTriggeredChambers() const;
	/**
	 * @brief Получить множество номеров сработавших камер
	 * @param triggChambers Множество, в которое будут загружны номера сработавших камер
	 */
	void loadTriggeredChambers(std::set<uintmax_t>& triggChambers) const;
  protected:
	TDCEvent(EventType type) : mType(type) {}
	void setTrekData(const UIntVector& data) { mTrekEvent = data; }
	void pushMsr(uint32_t word) { mTrekEvent.push_back(word); }
	const EventType mType;
  private:
	UIntVector		mTrekEvent;		/**< Вектор с сырыми записями. */
};

/**
 * @class TUEvent
 * @author frostoov
 * @date 03/15/15
 * @file tuevent.hpp
 * @brief Событие TDC-Ураган
 */
class TUEvent : public TDCEvent {
	friend class DataSet;
	/** Типы событий */
  public:
	using ChamberEvent	= std::array<UIntVector, 4>;
	using TrekEvent		= std::unordered_map<size_t,ChamberEvent>;
  public:
	TUEvent() : TDCEvent(EventType::uragan) {}
	/**
	 * @brief Получение события с урагана
	 * @return Ссылка на константную структура с данными события УРАГАНа
	 */
	const UraganEvent& getUraganEvent() const { return mUraganEvent; }
  protected:
	void setUraganData	(const UraganEvent& data) { mUraganEvent = data; }
  private:
	UraganEvent mUraganEvent;	/**< Событие с урагана */
};


/**
 * @class AbstractEventHandler
 * @author frostoov
 * @date 03/15/15
 * @file tuevent.hpp
 * @brief Класс задает интерфейс для обработки DataSet-ов
 */
class AbstractEventHandler {
  public:
	virtual ~AbstractEventHandler() {}
	/**
	 * @brief Обработка единичного события
	 * @param event Событие, которое будет обработано
	 */
	virtual void handleEvent(const TUEvent& TUEvent) = 0;
	/**
	 * @brief Завершение обработки данных
	 */
	virtual void flush() = 0;
  protected:
	AbstractEventHandler() {}
};

} //tudata

#endif // TDSEVENT_HPP_uraganData
