#ifndef TREKHANDLER_HPP
#define TREKHANDLER_HPP

#include "chamber.hpp"
#include "tdcdata/event.hpp"

namespace trek {

class TrekHandler
{
	using TrackDesc  = ChamberEventHandler::TrackDesc;
	using Octahedron = vecmath::Octahedron;
	using Vec3       = vecmath::Vec3;
	using Line2      = vecmath::Line2;
	using Line3      = vecmath::Line3;
	using TUEvent	 = tdcdata::TUEvent;
	using ChamberMap = std::unordered_map<uintmax_t, Chamber>;
public:
	TrekHandler(const ChamberConfig& config, uint32_t pedestal, double speed);
	const ChamberMap& getChambers() const { return mChambers; }

	void     setSpeed(double speed) {mEventHandler.setSpeed(speed);}
	void     setPedestal(uint32_t pedestal) {mEventHandler.setPedestal(pedestal);}
	void     loadEvent(const TUEvent& rawEvent);
	void     loadChambers(const ChamberConfig& chams);
	void     createTrack();
	static Line3 createTrack(const Chamber& cham1, const Chamber& cham2);
	static Line3 createTrack(const Line2& track1, const ChamberPoints& pos1,
							 const Line2& track2, const ChamberPoints& pos2);

	void loadVertices	(std::vector<float>& data) const;
	void loadLineColors	(std::vector<float>& data) const;
	void loadLineFace	(std::vector<unsigned>&	face, unsigned start) const;

	bool hasTrack() const { return mHasTrack; }
	const Line3& getTTrack() const { return mTTrack; }
	const Line3& getUTrack() const { return mUTrack; }
private:
	ChamberMap			mChambers;
	ChamberEventHandler mEventHandler;
	Line3 mTTrack;
	Line3 mUTrack;
	bool  mHasTrack;
};

}

#endif // TREKHANDLER_HPP
