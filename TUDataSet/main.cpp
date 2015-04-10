#include <stdexcept>
#include <iostream>
#include <cstring>

#include "tracktest.hpp"
#include "tdcdata/dataset.hpp"
#include "tdcdata/paramshandler.hpp"
#include "tdcdata/eventhandler.hpp"
#include "configparser/chamberconfigparser.hpp"
#include "tools.hpp"
#include "configparser/flagparser.hpp"

using tdcdata::DataSet;
using tdcdata::ParametersHandler;
using tdcdata::EventHandler;
using std::ios_base;
using std::exception;
using std::cout;
using std::cin;
using std::endl;
using std::string;

void panic(const string& message) {
	cout << message << endl;
	exit(0);
}

void help() {
	cout << "Использование: TUDataSet [ПАРАМЕТР] ... [ИМЯ ДИРЕКТОРИИ/ФАЙЛА].\n"
	     << "-pedestal=value        устанавливает пьедестал\n"
	     << "-speed=число           устанавливает скорость\n"
	     << "-matrix                вывод матриц\n"
	     << "-track                 вывод текстового описания треков\n"
	     << "-listing               вывод листинга событий\n"
	     << endl;
}

int main(int argc, char* argv[]) {
	ios_base::sync_with_stdio(false);
	AppFlags flags;
	try {
		flags = loadFlags(argc, argv);
		if(flags.needHelp) help();
	} catch(const exception& e) {
		cout << "ParsingFlags: " << e.what() << endl;
		help();
		return 0;
	}

	DataSet buffer;
	ChamberConfigParser parser;
	try {
		parser.load("chambers.conf");
	} catch (const exception& e) {
		cout << "Reading chambers.conf: " << e.what() << endl;
	}
	cout << "Config has been read" << endl;

	if(flags.handleFlags.tracks ||
	        !(flags.handleFlags.tracks || flags.handleFlags.listing ||flags.handleFlags.matrix)) {
		if (flags.pedestal == 0 || flags.speed == 0) {
			ParametersHandler handler;
			try {
				cout << "Finding data parameters" << endl;
				handleData(flags.path, buffer, handler);
				flags.pedestal = handler.getPedestal();
				flags.speed    = handler.getSpeed();
			} catch(const exception& e) {
				cout << e.what() << endl;
			}
		}
		cout << "pedestal = " << flags.pedestal << endl;
		cout << "speed = "    << flags.speed << endl;
	}

	if(flags.handleFlags.tracks || flags.handleFlags.listing ||flags.handleFlags.matrix) {
		EventHandler eventHandler(parser.getConfig(), flags.pedestal, flags.speed);
		eventHandler.setFlags(flags.handleFlags);
		try {
			cout << "processing data..." << endl;
			handleData(flags.path, buffer, eventHandler);
		} catch(const exception& e) {
			cout << e.what() << endl;
		}
	}
	cout << "Press [enter] to exit" << endl;
	cin.ignore();
	return 0;
}
