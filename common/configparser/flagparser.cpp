#include <stdexcept>
#include <iostream>
#include "flagparser.hpp"

using std::runtime_error;
using std::string;
using std::begin;
using std::end;

FlagParser::FlagParser(int argc, char* argv[]) {
	string flag;
	for(int i = 1; i < argc; ++i) {
		size_t j = 0;
		while(argv[i][j] != '\0' && argv[i][j] != '-') ++j;
		while(argv[i][j] == '-' && j < 2) ++j;
		if(argv[i][j] == '\0' || !isalpha(argv[i][j]))
			throw runtime_error(string("Invalid flag: ") + argv[i]);

		flag.assign(argv[i] + j);
		auto pos = flag.find('=');
		if(pos != string::npos) {
			string key(begin(flag), begin(flag) + pos++);
			if(mNumFlags.count(key) != 0)
				throw runtime_error(string("Flags: duplicate flag: ") + key);
			string value(begin(flag) + pos, end(flag) );
			mNumFlags.insert({key, value});
		} else {
			if(mSimpleFlags.count(flag) != 0)
				throw runtime_error(string("Flags: duplicate flag: ") + flag);
			mSimpleFlags.insert(flag);
		}
	}
}

double FlagParser::parseNumber(const std::string& flag, double defaultValue) {
	auto record = mNumFlags.find(flag);
	if(record != std::end(mNumFlags))
		return std::stod(record->second);
	else
		return defaultValue;
}

string FlagParser::parseString(const string& flag, const string& defaultValue) {
	auto record = mNumFlags.find(flag);
	if(record != std::end(mNumFlags))
		return record->second;
	else
		return defaultValue;
}

bool FlagParser::parseFlag(const std::string& flag, bool defaultValue) {
	if(mSimpleFlags.count(flag))
		return true;
	else
		return defaultValue;
}
