#ifndef TUDATASET_TOOLS_HPP
#define TUDATASET_TOOLS_HPP

#include <cstdint>
#include <utility>
#include <string>
#include <boost/filesystem/path.hpp>

#include "tdcdata/dataset.hpp"
#include "tdcdata/event.hpp"

struct AppFlags {
	bool   tracks;
	bool   projections;
	bool   matrix;
	bool   listing;
	bool   parameters;
	boost::filesystem::path	dirPath;
};

void panic(const std::string& message);
void help();
uintmax_t handleData(const boost::filesystem::path& dirPath, tdcdata::DataSet& buffer,
                     std::vector<tdcdata::AbstractEventHandler*> handlers);
AppFlags loadFlags(int argc, char* argv[]);

#endif // TUDATASET_TOOLS_HPP

