#ifndef TUDATASET_TOOLS_HPP
#define TUDATASET_TOOLS_HPP

#include <cstdint>
#include <utility>
#include <string>

#include "tdcdata/dataset.hpp"
#include "tdcdata/event.hpp"

struct AppFlags {
	bool   tracks;
	bool   projections;
	bool   matrix;
	bool   listing;
	bool   parameters;
	std::string	dirPath;
};

void panic(const std::string& message);
void help();
uintmax_t handleData(const std::string& dirPath, tdcdata::DataSet& buffer,
                     std::vector<tdcdata::AbstractEventHandler*> handlers);
AppFlags loadFlags(int argc, char* argv[]);

#endif // TUDATASET_TOOLS_HPP

