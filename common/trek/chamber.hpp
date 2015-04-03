#ifndef CHAMBER_HPP
#define CHAMBER_HPP

#include "chamberhandler.hpp"
#include "tdcdata/event.hpp"
#include "configparser/chamberconfigparser.hpp"
#include "math/figures.hpp"

namespace trek {

class Chamber
{
	using ChamberEvent = tdcdata::TDCEvent::ChamberEvent;
	using TrackDesc = ChamberEventHandler::TrackDesc;
	using Octahedron = vecmath::Octahedron;
	using Vec3  = vecmath::Vec3;
	using Line2 = vecmath::Line2;
	using Line3 = vecmath::Line3;
	using Plane = vecmath::Plane;
public:
	Chamber(const ChamberPosition& position);

	void setSelected(bool flag) {mIsSelected = flag;}

	void loadVertices	(std::vector<float>& vertex) const;
	void loadPlgColors	(std::vector<float>& colors) const;
	void loadLineColors	(std::vector<float>& colors) const;
	void loadPlgFace	(std::vector<uint>& face, uint start) const;
	void loadLineFace	(std::vector<uint>&	face, uint start) const;

	void setChamberEvent(const ChamberEvent& event) { mEvent = event; }
	const ChamberEvent& getChamberEvent() const { return mEvent; }

	void setTrack(const TrackDesc& track) { mTrack = track; mHasTrack = true; }
	const TrackDesc& getTrack() const {
		if(mHasTrack)
			return mTrack;
		throw std::runtime_error("Chamber: getTrack: no track");
	}

	Chamber::Plane getTrackPlane();

	void resetData() {mIsHit = false; mHasTrack = false;}

	uint getPlane() const {return mPlane;}
	uint getGroup() const {return mGroup;}

	bool isHit()		 const { return mIsHit; }
	bool hasTrack() const { return mHasTrack; }
	bool isSelected()	 const { return mIsSelected; }
protected:
	Octahedron getOctahedron(const Vec3& dot1, const Vec3& dot2, const Vec3& dot3) const;
private:
	Octahedron   mOct;
	TrackDesc    mTrack;
	ChamberEvent mEvent;

	uint mPlane;
	uint mGroup;

	bool mIsHit;
	bool mHasTrack;
	bool mIsSelected;

	constexpr static double chamberWidth   = 500;
	constexpr static double chamberHeight  = 112;
	constexpr static double chamberLength  = 4000;

};

}

#endif // CHAMBER_HPP
