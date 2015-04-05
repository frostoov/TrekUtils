#include <fstream>
#include "chamberconfigparser.hpp"

using std::string;
using std::ifstream;
using std::ios_base;

void ChamberConfigParser::load(const string& fileName) {
	config.clear();

	ifstream configFile;
	configFile.exceptions ( std::ofstream::failbit | std::ofstream::badbit );
	configFile.open(fileName,ios_base::binary);

	string inputString;
	while(getline(configFile,inputString,'\n')) {
		ChamberPosition chPos;
		size_t ndc;
		if( parseLine(inputString,chPos, ndc) && !config.count(ndc))
			config.insert({ndc, chPos});
		if(configFile.eof())
			break;
	}
}

bool ChamberConfigParser::parseLine(string& inputString, ChamberPosition& chPos, size_t& ndc) {
	auto commentPos = inputString.find('#');
	if( commentPos != string::npos )
		inputString.resize( commentPos );

	if(inputString.empty())
		return false;
	string token;
	auto iter = begin(inputString);
	short n = 0;
	while( (iter = getToken(iter, end(inputString), token)) != end(inputString) ||
			!token.empty()) {
		if(token.empty())
			continue;
		switch(n) {
			case 0:
				ndc = stoul(token) - 1;
				break;
			case 1:
				chPos.group = stoul(token);
				break;
			case 2:
				chPos.plane = stoul(token);
				break;
			default:
				chPos.points[(n - 3)/3][(n - 3)%3] = stol(token);
				break;
		}
		++n;
	}
	if(n < 12)
		return false;
	else
		return true;
}
