#ifndef TREKHANDLER_HPP
#define TREKHANDLER_HPP

#include "chamber.hpp"
#include "tdcdata/event.hpp"

namespace trek {

class TrekHandler
{
	using Line3      = vecmath::Line3;
	using TUEvent	 = tdcdata::TUEvent;
	using ChamberMap = std::unordered_map<uintmax_t, Chamber>;
public:
	const ChamberMap& getChambers() const { return mChambers; }

	void setSpeed(double speed) {mEventHandler.setSpeed(speed);}
	void setPedestal(uint32_t pedestal) {mEventHandler.setPedestal(pedestal);}
	void loadEvent(const TUEvent& rawEvent);
	void loadChambers(const ChamberConfig& chams);
	void createTrek();
private:
	ChamberMap mChambers;
	ChamberEventHandler mEventHandler;
	Line3 mTrack;
	bool  mHasTrack;
};

}

#endif // TREKHANDLER_HPP
