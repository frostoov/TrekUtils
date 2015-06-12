#include "event.hpp"

namespace tdcdata {

using std::istream;
using std::ios_base;

TDCEvent::TrekEvent TDCEvent::getTrekEvent() const {
	TrekEvent trekEvent;
	loadTrekEvent(trekEvent);
	return trekEvent;
}

void TDCEvent::loadTrekEvent(TrekEvent& trekEvent) const {
	for(auto word : mTrekEvent) {
		auto cham = getChamber(word);
		auto wire = getWire(word);
		auto msr  = getMsr(word);
		if( !trekEvent.count(cham) )
			trekEvent.insert({cham, EventTimes()});
		trekEvent[cham][wire].push_back(msr);
	}
}

TDCEvent::EventTimes TDCEvent::getChamberEvent(size_t cham) const {
	EventTimes chamEvent;
	loadChamberEvent(chamEvent, cham);
	return chamEvent;
}

void TDCEvent::loadChamberEvent(TUEvent::EventTimes& chamEvent, size_t cham) const {
	for(auto word : mTrekEvent) {
		if(getChamber(word) != cham)
			continue;
		chamEvent.at(getWire(word)).push_back(getMsr(word));
	}
}

std::set<uintmax_t> TDCEvent::getTriggeredChambers() const {
	std::set<uintmax_t> triggChambers;
	loadTriggeredChambers(triggChambers);
	return triggChambers;
}


void TDCEvent::loadTriggeredChambers(std::set<uintmax_t>& triggChambers) const {
	for(auto word : mTrekEvent)
		triggChambers.insert(getChamber(word));
}

} //tudata
