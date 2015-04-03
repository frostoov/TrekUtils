#ifndef CHAMBERCONFIGPARSER_HPP
#define CHAMBERCONFIGPARSER_HPP

#include <unordered_map>
#include <array>
#include <cstdint>

#include "configparser.hpp"
#include "math/vec.hpp"

/**
 * @class ChamberPosition
 * @author frostoov
 * @date 03/18/15
 * @file chamberconfigparser.hpp
 * @brief Описание положения дрейфовой камеры
 */
struct ChamberPosition {
	std::array<vecmath::Vec3, 3> vertices;  /*!< Точки дрейфовой камеры */
	uint plane;								/*!< Номер плоскости дрейфовой камеры */
	uint group;								/*!< Номер группы дрейфовой камеры */
};

using AppConfig     = std::unordered_map<std::string, std::string>;
using ChamberConfig = std::unordered_map<uintmax_t, ChamberPosition>;

class ChamberConfigParser : public AbstractConfigParser {
  public:
	virtual void load(const std::string& fileName);
	virtual void save(const std::string&) {}
	const ChamberConfig& getConfig() const { return config; }
	ChamberConfig& getConfig() { return config; }
  protected:
	bool parseLine(std::string& str, ChamberPosition& chPos, size_t& ndc);
  private:
	ChamberConfig config;
};

#endif // CHAMBERCONFIGPARSER_HPP
