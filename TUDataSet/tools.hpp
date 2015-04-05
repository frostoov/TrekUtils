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
	bool		fullTrackFlag;
	bool		pedestalFlag;
	bool		speedFlag;
	bool		dirFlag;
	uint32_t	pedestal;
	double		speed;
	std::string	dirName;
};

void handleData(const std::string& path, tdcdata::DataSet& buffer,
				tdcdata::AbstractEventHandler& handler);
void panic(const std::string& message);
void loadFlags(int argc, char* argv[], AppFlags& flags);

#endif // TUDATASET_TOOLS_HPP

