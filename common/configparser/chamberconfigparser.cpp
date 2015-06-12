#include <fstream>
#include "chamberconfigparser.hpp"

using std::string;
using std::ostream;
using std::ifstream;
using std::ofstream;
using std::exception;
using std::istreambuf_iterator;
using nlohmann::json;
using trek::WireParameters;
using trek::ChamberParameters;
using trek::ChamberDescription;
using trek::ChamberConfig;
using trek::ChamberPoints;
using vecmath::Vec3;

ChamberConfigParser::ChamberConfigParser(uint32_t defaultOffset, double defaultSpeed)
	: mDefaultOffset(defaultOffset), mDefaultSpeed(defaultSpeed) { }

void ChamberConfigParser::load(const string& fileName) {
	mConfig.clear();
	ifstream configFile;
	configFile.exceptions(ifstream::failbit | ifstream::badbit );
	configFile.open(fileName, ifstream::binary);

	string jsonText({istreambuf_iterator<char>(configFile), istreambuf_iterator<char>()});

	auto config = json::parse(jsonText);
	if( config.count("chambers") ) {
		auto& chambers = config.at("chambers");
		for(const auto& chamber : chambers) {
			const auto& point1 = chamber.at("point1");
			const auto& point2 = chamber.at("point2");
			const auto& point3 = chamber.at("point3");

			ChamberPoints points = {{
					Vec3(point1.at(0), point1.at(1), point1.at(2)),
					Vec3(point2.at(0), point2.at(1), point2.at(2)),
					Vec3(point3.at(0), point3.at(1), point3.at(2)),
				}
			};
			ChamberParameters parameters{{
					WireParameters(mDefaultOffset, mDefaultSpeed),
					WireParameters(mDefaultOffset, mDefaultSpeed),
					WireParameters(mDefaultOffset, mDefaultSpeed),
					WireParameters(mDefaultOffset, mDefaultSpeed),
				}};
			if(chamber.count("speeds")) {
				const auto& speeds = chamber.at("speeds");
				for(size_t i = 0; i < parameters.size(); ++i)
					parameters.at(i).setSpeed( speeds.at(i) );
			}
			if(chamber.count("offsets")) {
				const auto& offsets = chamber.at("offsets");
				for(size_t i = 0 ; i < parameters.size(); ++i)
					parameters.at(i).setOffset( offsets.at(i) );
			}
			auto chamNumber = static_cast<size_t>(chamber.at("number")) - 1;
			uint32_t plane = chamber.at("plane");
			uint32_t group = chamber.at("group");
			mConfig.insert({chamNumber, ChamberDescription(points, parameters, plane, group)});
		}
	}
}

void ChamberConfigParser::save(const std::string& path) {
	json chamberArray;
	for(const auto& chamberDescPair : mConfig)
		chamberArray.push_back(createJson(chamberDescPair.second, chamberDescPair.first));
	json fullDoc {
		{"chambers", chamberArray}
	};
	ofstream stream;
	stream.exceptions(ofstream::badbit | ofstream::failbit);
	stream.open(path, ofstream::binary);

	stream << fullDoc.dump(4);
}

json ChamberConfigParser::createJson(const ChamberDescription& chamberDescription, uintmax_t number) {
	const auto& params = chamberDescription.getParameters();
	const auto& points = chamberDescription.getPoints();
	return {
		{"number", number},
		{"group",  chamberDescription.getGroup()},
		{"plane",  chamberDescription.getPlane()},
		{"point1", {points.at(0).x(), points.at(0).y(), points.at(0).z()}},
		{"point2", {points.at(1).x(), points.at(1).y(), points.at(1).z()}},
		{"point3", {points.at(2).x(), points.at(2).y(), points.at(2).z()}},
		{"offsets", {params.at(0).getOffset(), params.at(1).getOffset(), params.at(2).getOffset(), params.at(3).getOffset()}},
		{"speeds",  {params.at(0).getSpeed(), params.at(1).getSpeed(), params.at(2).getSpeed(), params.at(3).getSpeed()}},
	};
}
