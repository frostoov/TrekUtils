#ifndef TREKVIEWER_APPCONFIGPARSER_HPP
#define TREKVIEWER_APPCONFIGPARSER_HPP

#include <unordered_map>
#include "json.hpp"
#include "configparser/configparser.hpp"

class AppConfigParser : public AbstractConfigParser {
  public:
	AppConfigParser() {};
	AppConfigParser(const nlohmann::json&& defaultConf);

	virtual void load(const std::string& fileName) override;
	virtual void save(const std::string& fileName) override;

	const nlohmann::json& getConfig() const {return mConfig;}
  private:
	nlohmann::json mConfig;
};

#endif // TREKVIEWER_APPCONFIGPARSER_HPP
