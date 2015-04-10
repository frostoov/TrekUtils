#ifndef TREKVIEWER_APPCONFIGPARSER_HPP
#define TREKVIEWER_APPCONFIGPARSER_HPP

#include <unordered_map>
#include "configparser/configparser.hpp"


using AppConfig = std::unordered_map<std::string, std::string>;

class AppConfigParser : public AbstractConfigParser {
  public:
	AppConfigParser(const AppConfig&& defaultConf);

	virtual void load(const std::string& fileName) override;
	virtual void save(const std::string& fileName) override;

	AppConfig& getConfig() {return mConfig;}
	const AppConfig& getConfig() const {return mConfig;}
  protected:
	void parseString(const std::string& inputString, AppConfig& config);
  private:
	AppConfig mConfig;
};

#endif // TREKVIEWER_APPCONFIGPARSER_HPP
