#include <cstring>
#include <sstream>
#include <typeinfo>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <boost/filesystem.hpp>

#include "configparser/flagparser.hpp"
#include "tdcdata/dataset.hpp"
#include "tools.hpp"

using boost::filesystem::is_directory;
using boost::filesystem::directory_iterator;
using boost::filesystem::exists;
using boost::filesystem::path;
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

uintmax_t handleData(const boost::filesystem::path& dirPath, DataSet& buffer,
                     vector<AbstractEventHandler*> handlers) {
	if( is_directory(dirPath) ) {
		vector<path> filePaths;
		std::copy(directory_iterator(dirPath), directory_iterator(), back_inserter(filePaths));
		uintmax_t eventCount = 0;
		for(const auto& filePath : filePaths) {
			const auto& fileName = filePath.native();
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
