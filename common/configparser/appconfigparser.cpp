#include <fstream>
#include "appconfigparser.hpp"

using std::ifstream;
using std::ofstream;
using std::string;
using std::getline;
using std::istreambuf_iterator;
using nlohmann::json;

AppConfigParser::AppConfigParser(const json&& defaultConfig) {
	mConfig = std::move(defaultConfig);
}

void AppConfigParser::load(const string& fileName) {
	ifstream configFile;
	configFile.exceptions(ifstream::badbit);
	configFile.open(fileName, ifstream::binary);
	string jsonText({istreambuf_iterator<char>(configFile), istreambuf_iterator<char>()});
	mConfig = json::parse(jsonText);
}

void AppConfigParser::save(const string& fileName) {
	ofstream configFile;
	configFile.exceptions(ifstream::failbit | ifstream::badbit);
	configFile.open(fileName, ifstream::binary);
	configFile << mConfig.dump(4);
}
