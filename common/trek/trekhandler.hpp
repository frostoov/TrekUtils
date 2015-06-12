#ifndef TREKHANDLER_HPP
#define TREKHANDLER_HPP

#include "chamber.hpp"
#include "tdcdata/event.hpp"

namespace trek {

class TrekHandler {
	using Octahedron = vecmath::Octahedron;
	using Vec3       = vecmath::Vec3;
	using Line2      = vecmath::Line2;
	using Line3      = vecmath::Line3;
	using TUEvent	 = tdcdata::TUEvent;
	using ChamberMap = std::unordered_map<uintmax_t, Chamber>;
  public:
	TrekHandler(const ChamberConfig& config);
	const ChamberMap& getChambers() const { return mChambers; }

	void loadEvent(const TUEvent& rawEvent);
	void loadChambers(const ChamberConfig& chams);
	bool createTrack();

	bool hasTrack() const { return mHasTrack; }
	const Line3& getTTrack() const { return mTTrack; }
	const Line3& getUTrack() const { return mUTrack; }
  protected:
	static Line3 createTrack(const Chamber& cham1, const Chamber& cham2);
  private:
	ChamberMap mChambers;
	Line3      mTTrack;
	Line3      mUTrack;
	bool  mHasTrack;
};

}

#endif // TREKHANDLER_HPP
