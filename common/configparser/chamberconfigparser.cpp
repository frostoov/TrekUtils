#include <fstream>
#include "json.hpp"
#include "chamberconfigparser.hpp"

using std::string;
using std::ifstream;
using std::exception;
using std::istreambuf_iterator;
using nlohmann::json;
using trek::ChamberPosition;
using trek::ChamberConfig;
using trek::ChamberPoints;
using vecmath::Vec3;

void ChamberConfigParser::load(const string& fileName) {
	mConfig.clear();
	ifstream configFile;
	configFile.exceptions (ifstream::failbit | ifstream::badbit );
	configFile.open(fileName, ifstream::binary);

	string jsonText({istreambuf_iterator<char>(configFile), istreambuf_iterator<char>()});

	auto config = json::parse(jsonText);
	if(config.count("chambers") == 1) {
		auto& chambers = config.at("chambers");
		for(const auto& chamber : chambers) {
			auto& firstPoint  = chamber.at("first_point");
			auto& secondPoint = chamber.at("second_point");
			auto& thirdPoint  = chamber.at("third_point");

			ChamberPoints points = {{
					Vec3(double(firstPoint.at(0)),  double(firstPoint.at(1)),  double(firstPoint.at(2))),
					Vec3(double(secondPoint.at(0)), double(secondPoint.at(1)), double(secondPoint.at(2))),
					Vec3(double(thirdPoint.at(0)),  double(thirdPoint.at(1)),  double(thirdPoint.at(2)))
				}
			};
			mConfig[chamber.at("number")] = {
				points,
				chamber.at("plane"),
				chamber.at("group")
			};
		}
	}
}
