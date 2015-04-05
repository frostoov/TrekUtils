#include <stdexcept>
#include <iostream>

#include "tracktest.hpp"
#include "tdcdata/dataset.hpp"
#include "tdcdata/paramshandler.hpp"
#include "tdcdata/eventhandler.hpp"
#include "configparser/chamberconfigparser.hpp"
#include "tools.hpp"

using tdcdata::DataSet;
using tdcdata::ParametersHandler;
using tdcdata::EventHandler;
using std::ios_base;
using std::exception;
using std::cout;
using std::cin;
using std::endl;


int main(int argc, char* argv[]) {
	DataSet buffer;
	ios_base::sync_with_stdio(false);
	AppFlags flags{{false, false, false}, false, false, false, false, 0, 0, "./"};
	loadFlags(argc, argv, flags);

	ChamberConfigParser parser;
	try {
		parser.load("chambers.conf");
	} catch (const exception& e) {
		cout << e.what() << endl;
	}
	cout << "Config has been read" << endl;

	if(flags.handleFlags.tracks || flags.fullTrackFlag ||
			!(flags.handleFlags.tracks || flags.handleFlags.listing ||flags.handleFlags.matrix || flags.fullTrackFlag)) {
		if (!flags.pedestalFlag || !flags.speedFlag) {
			ParametersHandler handler;
			try {
				cout << "Finding data parameters" << endl;
				handleData(flags.dirName, buffer, handler);
				flags.pedestal = handler.getPedestal();
				flags.speed    = handler.getSpeed();
			} catch(const exception& e) {
				cout << e.what() << endl;
			}
		}
		cout << "pedestal = " << flags.pedestal << endl;
		cout << "speed = "    << flags.speed << endl;
	}
	if(flags.fullTrackFlag) {
		try {
			cout << "full track..." << endl;
			TrackTest testObj(parser.getConfig(), flags.pedestal, flags.speed);
			handleData(flags.dirName, buffer, testObj);
		} catch(const exception& e) {
			cout << e.what() << endl;
		}
	}

	if(flags.handleFlags.tracks || flags.handleFlags.listing ||flags.handleFlags.matrix) {
		EventHandler eventHandler(parser.getConfig(), flags.pedestal, flags.speed);
		eventHandler.setFlags(flags.handleFlags);
		try {
			cout << "processing data..." << endl;
			handleData(flags.dirName, buffer, eventHandler);
		} catch(const exception& e) {
			cout << e.what() << endl;
		}
	}
	cout << "Press [enter] to exit" << endl;
	cin.ignore();
	return 0;
}
