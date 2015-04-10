#ifndef TUDATASET_TOOLS_HPP
#define TUDATASET_TOOLS_HPP

#include <cstdint>
#include <utility>
#include <string>

#include "tdcdata/dataset.hpp"
#include "tdcdata/event.hpp"
#include "tdcdata/eventhandler.hpp"

struct AppFlags {
	tdcdata::EventHandler::HandleFlags handleFlags;
	uint32_t	pedestal;
	double		speed;
	std::string	path;
	bool needHelp;
};

void handleData(const std::string& path, tdcdata::DataSet& buffer,
                tdcdata::AbstractEventHandler& handler);
AppFlags loadFlags(int argc, char* argv[]);

#endif // TUDATASET_TOOLS_HPP

