#ifndef CHAMBER_HPP
#define CHAMBER_HPP

#include "chamberhandler.hpp"
#include "tdcdata/event.hpp"
#include "math/figures.hpp"

namespace trek {

class Chamber {
	using CoordSystem3 = vecmath::CoordSystem3;
	using ChamberEvent = tdcdata::TDCEvent::ChamberEvent;
	using Octahedron = vecmath::Octahedron;
	using Vec3  = vecmath::Vec3;
	using Line2 = vecmath::Line2;
	using Line3 = vecmath::Line3;
	using Plane = vecmath::Plane;
  public:
	Chamber(const ChamberPosition& position);
	void loadVertices	(std::vector<float>& vertex) const;
	void loadPlgColors	(std::vector<float>& colors) const;
	void loadLineColors	(std::vector<float>& colors) const;
	void loadPlgFace	(std::vector<unsigned int>& face, unsigned int start) const;
	void loadLineFace	(std::vector<unsigned int>&	face, unsigned int start) const;

	const Octahedron& octahedron() const { return mOct; }

	void setTrackProjection(const ChamberHandler& handler);
	void setTrackProjection(const ChamberHandler::TrackDesc& track) {
		mTrack = track;
		mHasTrack = true;
	}
	const Line2& getTrackProjection() const {
		if(mHasTrack)
			return mTrack.line;
		throw std::runtime_error("Chamber: getTrack: no track");
	}
	const ChamberHandler::TrackDesc& getTrackDesc() const {
		if(mHasTrack)
			return mTrack;
		throw std::runtime_error("Chamber: getTrack: no track");
	}
	void resetData() {
		mIsHit    = false;
		mHasTrack = false;
	}

	Plane getTrackPlane() const;
	static Plane getTrackPlane(const Line2& track, const ChamberPoints& pos);

	unsigned int getChamberPlane() const {return mPosition.plane;}
	unsigned int getChamberGroup() const {return mPosition.group;}
	const ChamberPoints& getPoints() const { return mPosition.points; }

	bool isHit()		 const { return mIsHit; }
	bool hasTrack()		 const { return mHasTrack; }

	static CoordSystem3 getChamberSystem(const ChamberPoints& pos);
	Line2 getUraganProjection(const Line3& track) const { return getUraganProjection(track, mChamberSystem); }
	Line2 getUraganProjection(Vec3 p1, Vec3 p2) const { return getUraganProjection(p1, p2, mChamberSystem); }
	static Line2 getUraganProjection(Line3 track, const CoordSystem3& system);
	static Line2 getUraganProjection(Vec3 p1, Vec3 p2, const CoordSystem3& system);
  protected:
	static Octahedron getOctahedron(const ChamberPoints& pos);
	static ChamberPoints getPoints(const Octahedron& oct);
  private:
	ChamberPosition mPosition;
	CoordSystem3    mChamberSystem;
	Octahedron      mOct;
	ChamberHandler::TrackDesc mTrack;

	bool mIsHit;
	bool mHasTrack;

	constexpr static double mChamberWidth   = 500;
	constexpr static double mChamberHeight  = 112;
	constexpr static double mChamberLength  = 4000;
};

}

#endif // CHAMBER_HPP
