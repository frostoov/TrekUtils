#ifndef CHAMBERCONFIGPARSER_HPP
#define CHAMBERCONFIGPARSER_HPP

#include <unordered_map>
#include <array>
#include <cstdint>

#include "configparser.hpp"
#include "trek/chamberhandler.hpp"

class ChamberConfigParser : public AbstractConfigParser {
  public:
	virtual void load(const std::string& fileName);
	virtual void save(const std::string&) {}
	const trek::ChamberConfig& getConfig() const { return mConfig; }
  private:
	trek::ChamberConfig mConfig;
};

#endif // CHAMBERCONFIGPARSER_HPP
