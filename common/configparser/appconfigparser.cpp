#include <fstream>
#include "appconfigparser.hpp"

using std::ifstream;
using std::ofstream;
using std::string;
using std::getline;

AppConfigParser::AppConfigParser(const AppConfig&& defaultConf) {
	mConfig = std::move(defaultConf);
}

void AppConfigParser::load(const string& fileName) {
	ifstream configFile;
	configFile.exceptions(ifstream::badbit);
	configFile.open(fileName, ifstream::binary);
	string inputString;

	while(getline(configFile, inputString, '\n')) {
		parseString(inputString, mConfig);
		if(configFile.eof())
			return;
	}
}

void AppConfigParser::save(const string& fileName) {
	ofstream configFile;
	configFile.exceptions(ifstream::failbit | ifstream::badbit);
	configFile.open(fileName, ifstream::binary);
	for(const auto& record : mConfig)
		configFile << record.first << " = " << record.second << '\n';
}

void AppConfigParser::parseString(const string& inputString, AppConfig& config) {
	if(inputString.empty())
		return;
	string token;
	auto iter = getToken(begin(inputString), end(inputString), token);
	if(config.count(token) && iter != end(inputString)) {
		auto& record = config.at(token);
		iter = getToken(iter, end(inputString), token);
		if(token == "=" && iter != end(inputString)) {
			iter = getToken(iter, end(inputString), token);
			record = token;
		}
	}
}

