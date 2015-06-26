#include <cstring>
#include <sstream>
#include <typeinfo>
#include <iostream>
#include <stdexcept>
#include <system_error>

#include <QString>
#include <QDir>

#include "configparser/flagparser.hpp"
#include "tdcdata/dataset.hpp"
#include "tools.hpp"

using std::string;
using std::vector;
using std::ios_base;
using sstream  = std::stringstream;
using std::cout;
using std::endl;
using std::runtime_error;
using std::ifstream;
using std::exception;
using std::vector;
using tdcdata::DataSet;
using tdcdata::AbstractEventHandler;

void panic(const string& message) {
	cout << message << endl;
	exit(0);
}

void help() {
	cout << "Использование: TUDataSet [ПАРАМЕТРЫ]\n"
	     << "-path=                 директория с данными\n"
	     << "-matrix                вывод матриц\n"
	     << "-tracks                вывод текстового описания проекций между треками\n"
	     << "-projections           вывод разницы углов между треками\n"
	     << "-listing               вывод листинга событий\n"
	     << "-parameters            нахождение характеристки дрейфовых камер\n"
	     << endl;
}

AppFlags loadFlags(int argc, char* argv[]) {
	FlagParser parser(argc, argv);

	AppFlags flags;
	flags.tracks = parser.parseFlag("tracks", false);
	flags.matrix = parser.parseFlag("matrix", false);
	flags.listing = parser.parseFlag("listing", false);
	flags.projections = parser.parseFlag("projections", false);
	flags.parameters  = parser.parseFlag("parameters", false);

	flags.dirPath = parser.parseString("path", ".");
	if( parser.parseFlag("help", false) )
		help();
	return flags;
}

uintmax_t handleData(const string& dirPath, DataSet& buffer,
                     vector<AbstractEventHandler*> handlers) {

	QDir director(QString::fromStdString(dirPath));
	if(director.exists()) {
		QStringList fileList = director.entryList();
		uintmax_t eventCount = 0;
		for(const auto& filePath : fileList) {
			const auto fileName = filePath.toStdString();
			if(DataSet::checkExtension(fileName)) {
				try {
					buffer.read(fileName);
				} catch(const exception& e) {
					std::cout << "Failed read " << fileName << ": " << e.what() << '\n';
				}
				eventCount += buffer.size();
				for(auto& handler : handlers) {
					try {
						for(const auto& event : buffer)
							handler->handleEvent(event);
					} catch(const exception& e) {
						std::cout << "Failed handle " << fileName << ": " << e.what() << '\n';
					}
				}
			}
		}
		for(auto& handler : handlers)
			handler->flush();
		return eventCount;
	} else
		throw runtime_error("handleData: path does not indicate directory");
}
