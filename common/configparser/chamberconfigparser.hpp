#ifndef CHAMBERCONFIGPARSER_HPP
#define CHAMBERCONFIGPARSER_HPP

#include <unordered_map>
#include <array>
#include <cstdint>

#include "configparser.hpp"
#include "trek/chamberhandler.hpp"

using AppConfig     = std::unordered_map<std::string, std::string>;

class ChamberConfigParser : public AbstractConfigParser {
	using ChamberPosition = trek::ChamberPosition;
	using ChamberConfig   = trek::ChamberConfig;
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
