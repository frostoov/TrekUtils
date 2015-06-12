#ifndef CHAMBERCONFIGPARSER_HPP
#define CHAMBERCONFIGPARSER_HPP

#include <unordered_map>
#include <ostream>
#include <array>
#include <cstdint>
#include "json.hpp"

#include "configparser.hpp"
#include "trek/chamberhandler.hpp"

class ChamberConfigParser : public AbstractConfigParser {
  public:
	ChamberConfigParser(uint32_t defaultOffset, double defaultSpeed);
	virtual void load(const std::string& fileName);
	virtual void save(const std::string& path);
	void setConfig(const trek::ChamberConfig& config) {mConfig = config;}
	const trek::ChamberConfig& getConfig() const { return mConfig; }
  protected:

	nlohmann::json createJson(const trek::ChamberDescription& chamberDescription, uintmax_t number);
  private:
	trek::ChamberConfig mConfig;
	uint32_t mDefaultOffset;
	double   mDefaultSpeed;
};

#endif // CHAMBERCONFIGPARSER_HPP
