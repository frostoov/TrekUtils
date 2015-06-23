#ifndef LISTINGHANDLER_HPP
#define LISTINGHANDLER_HPP

#include "tdcdata/event.hpp"

class ListingHandler : public tdcdata::AbstractEventHandler {
	using StreamsMap = std::unordered_map<std::uintmax_t, std::ofstream*>;
  public:
	ListingHandler(const std::string& dirPath = ".");
	~ListingHandler();

	void setPath(const std::string& dirPath);

	void handleEvent(const tdcdata::TUEvent& event) override;
	void flush() override;
  protected:
	void printListing(const tdcdata::TUEvent& event);
	void createListingStream(uintmax_t cham);
  private:
	StreamsMap  mStreams;
	std::string mDirPath;
};

#endif // LISTINGHANDLER_HPP
