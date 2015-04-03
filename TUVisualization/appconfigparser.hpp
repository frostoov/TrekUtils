#ifndef APPCONFIGPARSER_HPP
#define APPCONFIGPARSER_HPP

#include <unordered_map>
#include "configparser/configparser.hpp"


class AppConfigParser : public AbstractConfigParser {
	using AppConfig = std::unordered_map<std::string, std::string>;
  public:
	AppConfigParser();

	virtual void load(const std::string& fileName) override;
	virtual void save(const std::string& fileName) override;

	AppConfig& getConfig() {return mConfig;}
	const AppConfig& getConfig() const {return mConfig;}
  protected:
	void parseString(const std::string& inputString, AppConfig& config);
  private:
	AppConfig mConfig;
};

#endif // APPCONFIGPARSER_HPP
