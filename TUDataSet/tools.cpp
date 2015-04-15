#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <typeinfo>
#include <iostream>
#include <stdexcept>
#include <system_error>

#include "configparser/flagparser.hpp"
#include "tdcdata/dataset.hpp"
#include "tools.hpp"

using std::string;
using std::ios_base;
using sstream  = std::stringstream;
using std::cout;
using std::endl;
using std::runtime_error;
using std::ifstream;
using std::exception;
using tdcdata::DataSet;
using tdcdata::AbstractEventHandler;

void panic(const string& message) {
	cout << message << endl;
	exit(0);
}

void help() {
	cout << "Использование: TUDataSet [ПАРАМЕТР] ... [ИМЯ ДИРЕКТОРИИ/ФАЙЛА].\n"
	     << "-pedestal=int          устанавливает пьедестал\n"
	     << "-speed=float           устанавливает скорость\n"
	     << "-matrix                вывод матриц\n"
	     << "-tracks                вывод текстового описания проекций между треками\n"
	     << "-projections           вывод разницы углов между треками"
	     << "-listing               вывод листинга событий\n"
	     << endl;
}

AppFlags loadFlags(int argc, char* argv[]) {
	FlagParser parser(argc, argv);

	AppFlags flags;
	flags.tracks = parser.parseFlag("tracks", false);
	flags.matrix = parser.parseFlag("matrix", false);
	flags.listing = parser.parseFlag("listing", false);
	flags.projections = parser.parseFlag("projections", false);

	flags.path = parser.parseString("path", "./");
	flags.speed = parser.parseNumber("speed",0);
	flags.pedestal = parser.parseNumber("pedestal", 0);
	if( parser.parseFlag("help", false) )
		help();
	return flags;
}

void handleData(const string& path, DataSet& buffer, std::vector<AbstractEventHandler*> handlers) {
	struct stat fileStat;
	stat(path.c_str(), &fileStat);
	if(S_ISDIR(fileStat.st_mode)) {
		auto dir = opendir( path.c_str() );
		if(!dir)
			throw runtime_error("handleData: cannot open directory");
		dirent* ent;

		while((ent = readdir(dir))) {
			string fileName(path);
			fileName.append("/");
			fileName.append(ent->d_name);
			if(DataSet::checkExtension(fileName)) {
				try {
					buffer.read(fileName);
				} catch(const exception& e) {
					std::cout << "Failed read " << ent->d_name << ": " << e.what() << '\n';
				}
				for(auto& handler : handlers) {
					try {
						for(const auto& event : buffer)
							handler->handleEvent(event);
					} catch(const exception& e) {
						std::cout << "Failed handle "<< ent->d_name << ": " << e.what() << '\n';
					}
				}
			}
		}
		for(auto& handler : handlers)
			handler->flush();
		closedir(dir);
	} else
		throw runtime_error("handleData: path does not indicate directory");
}
