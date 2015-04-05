#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <iostream>

#include "tdcdata/dataset.hpp"
#include "tools.hpp"

using std::string;
using sstream  = std::stringstream;
using std::cout;
using std::endl;
using std::runtime_error;
using std::ifstream;
using std::exception;
using tdcdata::DataSet;
using tdcdata::AbstractEventHandler;

void help() {
	cout << "Использование: TUDataSet [ПАРАМЕТР] ... [ИМЯ ДИРЕКТОРИИ/ФАЙЛА].\n"
		 << "-p, --pedestal число   устанавливает пьедестал\n"
		 << "-s, --speed    число   устанавливает скорость\n"
		 << "-m, --matrix           вывод матриц\n"
		 << "-t, --track            вывод текстового описания треков\n"
		 << "-l, --listing          вывод листинга событий\n"
		 << endl;
}

void panic(const string& message) {
	cout << message << endl;
	exit(0);
}

void loadFlags(int argc, char* argv[], AppFlags& flags) {
	for (int i = 1; i < argc; ++i) {
		if(!strcmp(argv[i],"--help") || !strcmp(argv[i],"-h")) {
			help();
			exit(0);
		}
		if(!strcmp(argv[i], "-p") || !strcmp(argv[i], "--pedestal")) {
			if (++i < argc && !flags.pedestalFlag) {
				flags.pedestal = strtoul(argv[i], nullptr, 10);
				flags.pedestalFlag = true;
			} else
				panic("Pedestal is already set");
		} else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--speed")) {
			if (++i < argc && !flags.speedFlag) {
				flags.speed= strtod(argv[i], nullptr);
				flags.speedFlag = true;
			} else
				panic("Speed is already set");
		} else if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--matrix")) {
			if (!flags.handleFlags.matrix)
				flags.handleFlags.matrix = true;
			else
				panic("Matrix is already set");
		} else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--track")) {
			if (!flags.handleFlags.tracks)
				flags.handleFlags.tracks = true;
			else
				panic("Tracks is already set");
		} else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--listing")) {
			if (!flags.handleFlags.listing)
				flags.handleFlags.listing = true;
			else
				panic("Listing is already set");
		}  else if (*argv[i] != '-' && !flags.dirFlag) {
			flags.dirName = argv[i];
			flags.dirFlag = true;
		} else if (!strcmp(argv[i], "-T") || !strcmp(argv[i], "--fulltrack")) {
			if (!flags.fullTrackFlag)
				flags.fullTrackFlag = true;
			else
				panic("Full Track is already set");
		} else
			panic("Unknown flag");
	}

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
				buffer.append(fileName);
				if(buffer.size() > 30000) {
					for(const auto& event : buffer)
						handler.handleEvent(event);
					buffer.clear();
				}
			} catch(const exception& e) {
				errors << "Failed handle " << ent->d_name << ": " << e.what() << '\n';
			}
			fileStream.clear(), fileStream.close();
		}
		for(const auto& event : buffer)
			handler.handleEvent(event);
		handler.flush();
		closedir(dir);
		string err(errors.str());
		if( !err.empty() )
			throw runtime_error(err);
	} else
		throw runtime_error("handleData: path does not indicate directory");
}
