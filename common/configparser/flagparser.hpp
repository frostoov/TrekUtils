#ifndef FLAGPARSER_HPP
#define FLAGPARSER_HPP

#include <string>
#include <unordered_set>
#include <unordered_map>

class FlagParser {
	using BoolFlags   = std::unordered_set<std::string>;
	using StringFlags = std::unordered_map<std::string, std::string>;
  public:
	FlagParser(int argc, char* argv[]);
	double parseNumber(const std::string& flag, double defaultValue);
	std::string parseString(const std::string& flag, const std::string&  defaultValue);
	bool parseFlag(const std::string& flag, bool defaultValue);
  private:
	BoolFlags mSimpleFlags;
	StringFlags    mNumFlags;
};

#endif // FLAGPARSER_HPP
