#include <stdexcept>
#include <iostream>
#include <cstring>

#include <QDir>

#include "tdcdata/dataset.hpp"

#include "eventhandler/matrixhandler.hpp"
#include "eventhandler/listinghandler.hpp"
#include "eventhandler/trackshandler.hpp"
#include "eventhandler/parametershandler.hpp"

#include "configparser/chamberconfigparser.hpp"
#include "configparser/appconfigparser.hpp"
#include "configparser/flagparser.hpp"

#include "tools.hpp"

using tdcdata::DataSet;
using tdcdata::AbstractEventHandler;
using std::ios_base;
using std::exception;
using std::cout;
using std::cin;
using std::endl;
using std::flush;
using std::string;
using std::vector;

string createDir(const std::string& dirPath) {
	QDir directory(QDir::currentPath());
	if(directory.mkpath(QString::fromStdString(dirPath)) == false)
		throw std::runtime_error("Cannot create directory: " + dirPath);
	return QDir(QString::fromStdString(dirPath)).absolutePath().toStdString();
}

int main(int argc, char* argv[]) {
	ios_base::sync_with_stdio(false);

	if(argc == 1) {
		help();
		exit(0);
	}

	AppFlags flags;
	try {
        cout << "Parsing flags: " << flush;
		flags = loadFlags(argc, argv);
		cout << "success" << endl;
	} catch(const exception& e) {
		cout << "error" << endl;
		help();
		exit(0);
	}

	struct AppConfig {
		double   defaultSpeed;
		uint32_t defaultOffset;
		bool     needFindParams;
	} appConfig{0, 0, false};

	AppConfigParser appParser({
		{"speed",  appConfig.defaultSpeed},
		{"offset", appConfig.defaultOffset},
		{"auto",   appConfig.needFindParams},
	});
	try {
		cout << "Reading TUDataSet.conf: " << flush;
		appParser.load("TUDataSet.conf");
		const auto& config = appParser.getConfig();
		appConfig.defaultOffset  = config.at("offset");
		appConfig.defaultSpeed   = config.at("speed");
		appConfig.needFindParams = config.at("auto");
		cout << "success" << endl;
	} catch (const exception& e) {
		cout << "error: " << e.what() << endl;
		exit(1);
	}

	ChamberConfigParser parser(appConfig.defaultOffset, appConfig.defaultSpeed);
	try {
		cout << "Reading chambers.conf: " << flush;
		parser.load("chambers.conf");
		cout << "success" << endl;
	} catch (const exception& e) {
		cout << "error: " << e.what() << endl;
		exit(1);
	}

	DataSet buffer;
	if( (appConfig.needFindParams && (flags.matrix || flags.projections)) || flags.parameters) {
		auto parameterHandler = new ParametersHandler;
		vector<AbstractEventHandler*> handlers{parameterHandler};
		try {
			handleData(flags.dirPath, buffer, handlers);
			auto chamberConfig = parser.getConfig();
			const auto& parameters = parameterHandler->getParameters();
			for(const auto& chamberParametersPair : parameters) {
				const auto  chamberNumber = chamberParametersPair.first;
				if(chamberConfig.count(chamberNumber)) {
					const auto& chamberParameters = chamberParametersPair.second;
					chamberConfig.at(chamberNumber).setParameters(chamberParameters);
				}
			}
			parser.setConfig(chamberConfig);
			cout << "Parameter was found" << endl;
			if(flags.parameters) {
				cout << "Writing new config to chambers.new.conf" << endl;
				parser.save("chambers.new.conf");
			}
		} catch(const exception& e) {
			cout << "Finding parameters: " << e.what() << endl;
			exit(1);
		}
		delete parameterHandler;
	}
	for(const auto& desc : parser.getConfig()) {
		const auto& params = desc.second.getParameters();
		cout << "Chamber : " << desc.first + 1 << endl;
		cout << "Offset  : [" << params.at(0).getOffset() << ", "
		     << params.at(1).getOffset() << ", "
		     << params.at(2).getOffset() << ", "
		     << params.at(3).getOffset() << "]\n";
		cout << "Speed   : [" << params.at(0).getSpeed() << ", "
		     << params.at(1).getSpeed() << ", "
		     << params.at(2).getSpeed() << ", "
		     << params.at(3).getSpeed() << "]\n";
		cout << "====" << endl;
	}

	vector<AbstractEventHandler*> handlers;
	if(flags.listing)
		handlers.push_back(new ListingHandler(createDir(QDir::currentPath().toStdString() + "/listings")));
	if(flags.matrix )
		handlers.push_back(new MatrixHandler(parser.getConfig(), createDir(QDir::currentPath().toStdString() + "/matrices")));
	if(flags.projections || flags.tracks) {
		auto tracksHandler = new TracksHandler(parser.getConfig(), createDir(QDir::currentPath().toStdString() + "/tracks"));
		tracksHandler->needProjection(flags.projections);
		tracksHandler->needTracks(flags.tracks);
		handlers.push_back(tracksHandler);
	}

	if(handlers.empty() == false) {
		try {
			cout << "processing data..." << endl;
			auto eventCount = handleData(flags.dirPath, buffer, handlers);
			cout << "Event count = " << eventCount  << endl;
		} catch(const exception& e) {
			cout << e.what() << endl;
		}
	}
    for(auto& handler : handlers)
        delete handler;
	return 0;
}
