#include <iomanip>
#include <fstream>
#include "listinghandler.hpp"


using tdcdata::TUEvent;
using std::setw;
using std::setfill;
using std::ofstream;
using std::to_string;

ListingHandler::~ListingHandler() {
	for(auto& stream : streams)
		delete stream.second;
	streams.clear();
}

void ListingHandler::handleEvent(const TUEvent& event) {
	printListing(event);
}

void ListingHandler::flush() {
	for(auto& stream : streams) {
		if(stream.second)
			stream.second->flush();
	}
}

void ListingHandler::printListing(const TUEvent& event) {
	auto trekEvent = event.getTrekEvent();
	for(const auto& chamEvent : trekEvent) {
		//Если для камеры нет потока, то создаем его
		if(!streams.count(chamEvent.first))
			createListingStream(streams, chamEvent.first);

		auto& str = *streams.at(chamEvent.first);
		size_t depth = 0;
		for(const auto& wireData : chamEvent.second) {
			if(wireData.size() > depth)
				depth = wireData.size();
		}
		for(size_t i = 0; i < depth; ++i) {
			for(const auto& wireData : chamEvent.second) {
				if(i < wireData.size())
					str << setw(8) << setfill(' ') << wireData.at(i) << '\t';
				else
					str << setw(8) << setfill('-') << "-" << '\t';
			}
			str << '\n';
		}
	}
}

void ListingHandler::createListingStream(StreamsMap& streams, uintmax_t cham) {
	if(!streams.count(cham)) {
		auto str = new ofstream;
		str->exceptions( ofstream::failbit | ofstream::badbit );
		str->open("listing" + to_string(cham+1) + ".txt", ofstream::binary);

		*str << "Chamber №" << cham+1 << '\n';
		for(auto i = 0; i < 4 ; ++i)
			*str << setw(6) << setfill(' ') << "WIRE "
			     << setw(2) << setfill('0') << i
			     << setw(1) << setfill(' ') << '\t';
		*str << '\n';

		streams[cham] = str;
	}
}

