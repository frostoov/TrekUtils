#ifndef LISTINGHANDLER_HPP
#define LISTINGHANDLER_HPP


#include "tdcdata/event.hpp"

class ListingHandler : public tdcdata::AbstractEventHandler {
	using StreamsMap       = std::unordered_map<std::uintmax_t, std::ofstream*>;
  public:
	~ListingHandler();

	void handleEvent(const tdcdata::TUEvent& event) override;
	void flush() override;
  protected:
	void printListing(const tdcdata::TUEvent& event);
	void createListingStream(StreamsMap& streams, uintmax_t cham);
  private:
	StreamsMap streams;
};

#endif // LISTINGHANDLER_HPP
