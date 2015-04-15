#include <stdexcept>
#include <iostream>
#include <cstring>

#include "tracktest.hpp"
#include "tdcdata/dataset.hpp"
#include "eventhandler/paramshandler.hpp"

#include "eventhandler/matrixhandler.hpp"
#include "eventhandler/listinghandler.hpp"
#include "eventhandler/trackshandler.hpp"

#include "configparser/chamberconfigparser.hpp"
#include "tools.hpp"
#include "configparser/flagparser.hpp"

using tdcdata::DataSet;
using tdcdata::ParametersHandler;
using tdcdata::AbstractEventHandler;
using std::ios_base;
using std::exception;
using std::cout;
using std::cin;
using std::endl;
using std::string;

int main(int argc, char* argv[]) {
	if(argc == 1) {
		help();
		exit(0);
	}
	ios_base::sync_with_stdio(false);
	AppFlags flags;
	try {
		flags = loadFlags(argc, argv);
	} catch(const exception& e) {
		cout << "ParsingFlags: " << e.what() << endl;
		help();
		exit(0);
	}

	DataSet buffer;
	ChamberConfigParser parser;
	try {
		parser.load("chambers.conf");
	} catch (const exception& e) {
		cout << "Reading chambers.conf: " << e.what() << endl;
	}
	cout << "Config has been read" << endl;

	if((flags.tracks || flags.projections) &&
	        (flags.pedestal == 0 || flags.speed == 0) ) {
		auto handler = new ParametersHandler;
		std::vector<AbstractEventHandler*> handlers{handler};
		try {
			cout << "Finding data parameters" << endl;
			handleData(flags.path, buffer, handlers);
			flags.pedestal = handler->getPedestal();
			flags.speed    = handler->getSpeed();
		} catch(const exception& e) {
			cout << e.what() << endl;
		}
		for(auto& handler : handlers) delete handler;
	}
	cout << "pedestal = " << flags.pedestal << endl;
	cout << "speed = "    << flags.speed << endl;

	std::vector<AbstractEventHandler*> handlers;
	if(flags.listing) handlers.push_back(new ListingHandler);
	if(flags.matrix ) handlers.push_back(new MatrixHandler);
	if(flags.projections || flags.tracks) {
		auto tracksHandler = new TracksHandler(parser.getConfig(), flags.pedestal, flags.speed);
		tracksHandler->needProjection(flags.projections);
		tracksHandler->needTracks(flags.tracks);
		handlers.push_back(tracksHandler);
	}

	if(handlers.empty() == false) {
		try {
			cout << "processing data..." << endl;
			handleData(flags.path, buffer, handlers);
		} catch(const exception& e) {
			cout << e.what() << endl;
		}
	}
	for(auto& handler : handlers) delete handler;
	cout << "Press [enter] to exit" << endl;
	cin.ignore();
	return 0;
}
