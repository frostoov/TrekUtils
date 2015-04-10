#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <typeinfo>
#include <iostream>

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

AppFlags loadFlags(int argc, char* argv[]) {
	FlagParser parser(argc, argv);

	AppFlags flags;
	flags.handleFlags.tracks = parser.parseFlag("tracks", false);
	flags.handleFlags.matrix = parser.parseFlag("matrix", false);
	flags.handleFlags.listing = parser.parseFlag("listing", false);

	flags.path = parser.parseString("path", "./");
	flags.speed = parser.parseNumber("speed",0);
	flags.pedestal = parser.parseNumber("pedestal", 0);
	flags.needHelp = parser.parseFlag("help", false);
	return flags;
}

void handleData(const string& path, DataSet& buffer, AbstractEventHandler& handler) {
	buffer.clear();
	struct stat fileStat;
	stat(path.c_str(), &fileStat);
	if(S_ISDIR(fileStat.st_mode)) {
		auto dir = opendir( path.c_str() );
		if(!dir)
			throw runtime_error("handleData: cannot open directory");
		dirent* ent;
		sstream errors;

		ifstream fileStream;
		fileStream.exceptions (ifstream::badbit);
		while((ent = readdir(dir))) {
			string fileName(path);
			fileName.push_back('/');
			fileName.append(ent->d_name);
			if(!DataSet::checkExtension(fileName))
				continue;
			try {
				buffer.read(fileName);
				for(const auto& event : buffer)
					handler.handleEvent(event);
			} catch(const exception& e) {
				std::cout << "Failed handle " << ent->d_name << ": " << e.what() << '\n';
			}
			fileStream.clear(), fileStream.close();
		}
		handler.flush();
		closedir(dir);
		string err(errors.str());
		if( !err.empty() )
			throw runtime_error(err);
	} else
		throw runtime_error("handleData: path does not indicate directory");
}
