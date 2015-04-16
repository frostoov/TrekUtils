#include <iostream>
#include "chamber.hpp"

namespace trek {

using vecmath::Line2;
using vecmath::Vec2;
using vecmath::CoordSystem3;
using std::runtime_error;

Chamber::Chamber(const ChamberPosition& position)
	: mPosition(position), mChamberSystem(getChamberSystem(position.points)),
	  mOct(getOctahedron(position.points)), mIsHit(false), mHasTrack(false)
{ }

CoordSystem3 Chamber::getChamberSystem(const ChamberPoints& pos) {
	auto xAxis = Vec3( pos[1] - pos[0] ).ort();
	auto zAxis = Vec3( pos[2] - pos[0] ).ort();
	return CoordSystem3(pos[0], xAxis, zAxis);
}

Line2 Chamber::getUraganProjection(Line3 track, const CoordSystem3& system) {
	auto& dot1 = track.dot();
	auto  dot2 = track.dot() + track.vec() * 10;
	return getUraganProjection(dot1, dot2 ,system);
}

Chamber::Line2 Chamber::getUraganProjection(Vec3 p1, Vec3 p2, const CoordSystem3& system) {
	system.convertTo(p1);
	system.convertTo(p2);

	return {{p1.x(), p1.y()}, {p2.x(), p2.y()}};
}

void Chamber::loadVertices(std::vector<float>& vertex) const {
	for(const auto& octVertex : mOct.vertices())
		for(const auto& val : octVertex)
			vertex.push_back(val);
}

void Chamber::loadPlgColors(std::vector<float>& colors) const {
	for(int k = 0 ; k < 8 ; ++k) {
		for(int i = 0; i < 3; ++i) {
			if(isHit()) {
				if(i == 0)
					colors.push_back(0.8);
				else
					colors.push_back(0.1);
			} else
				colors.push_back(0.5);
		}
		colors.push_back(1.0);
	}
}

void Chamber::loadLineColors(std::vector<float>& colors) const {
	for(int k = 0 ; k < 8 ; ++k) {
		for(int i = 0; i < 3; ++i)
			colors.push_back(0.0);
		colors.push_back(1.0);
	}
}

void Chamber::loadPlgFace(std::vector<unsigned int>& face, unsigned int start) const {
	for(int i = 0 ; i < 4 ; ++ i)
		face.push_back(start + i);
	for(int i = 4 ; i < 8; ++i)
		face.push_back(start + i);

	face.push_back(start + 1);
	face.push_back(start + 2);
	face.push_back(start + 6);
	face.push_back(start + 5);

	face.push_back(start + 0);
	face.push_back(start + 3);
	face.push_back(start + 7);
	face.push_back(start + 4);

	face.push_back(start + 0);
	face.push_back(start + 1);
	face.push_back(start + 5);
	face.push_back(start + 4);

	face.push_back(start + 3);
	face.push_back(start + 2);
	face.push_back(start + 6);
	face.push_back(start + 7);
}

void Chamber::loadLineFace(std::vector<unsigned int>& face, unsigned int start) const {
	for(int  k = 0; k <= 4; k += 4) {
		for(int i = k ; i < k + 3; ++i) {
			face.push_back(start + i);
			face.push_back(start + i + 1);
		}
		face.push_back(start + 3 + k);
		face.push_back(start + k);
	}
	for(int i = 0 ; i < 4 ; ++i) {
		face.push_back(start + i);
		face.push_back(start + i + 4);
	}
}

void Chamber::setTrackProjection(const ChamberHandler& handler) {
	if(handler.hasChamberTrack()) {
		setTrackProjection(handler.getChamberTrack());
		mHasTrack = true;
	} else
		mHasTrack = false;
	if(handler.hasChamberData())
		for(const auto& wireData : handler.getChamberEvent())
			if(!wireData.empty()) {
				mIsHit = true;
				break;
			}
}

Chamber::Plane Chamber::getTrackPlane() const {
	if(mHasTrack == false)
		throw runtime_error("Chamber: getTrackPlane: no track");
	return getTrackPlane(mTrack.line, mPosition.points);
}

Chamber::Plane Chamber::getTrackPlane(const Line2& track, const ChamberPoints& pos) {
	/*Вспомогательные векторы*/
	const auto p13 = pos.at(2) - pos.at(0);
	const auto p12 = pos.at(1) - pos.at(0);
	const auto wVec = (p12&p13).ort();

	auto pt1 = pos.at(0) + wVec* track.b();
	auto pt2 = pos.at(1) + wVec*(track.b() + track.k() * mChamberHeight);
	auto pt3 = pos.at(2) + wVec* track.b();

	return Plane(pt1, pt2, pt3);
}

Chamber::Octahedron Chamber::getOctahedron(const ChamberPoints& pos) {
	Octahedron ret;
	const int chamWidth = mChamberWidth/2;

	/*Вспомогательные векторы*/
	const auto p13 = pos.at(2) - pos.at(0);
	const auto p12 = pos.at(1) - pos.at(0);

	const auto wVec = (p12&p13).ort();

	/*Развертывание первого полигона*/
	ret[0] = pos.at(0) + wVec*chamWidth;
	ret[1] = pos.at(0) - wVec*chamWidth;
	ret[2] = ret[1] + p12;
	ret[3] = ret[0] + p12;

	/*Противоположный полигон*/
	for(char i = 4; i < 8; ++i)
		ret[i] = ret[i - 4] + p13;

	return ret;
}

ChamberPoints Chamber::getPoints(const Chamber::Octahedron& oct) {
	auto dot1 = (oct[0] + oct[1])*0.5;
	auto dot2 = (oct[2] + oct[3])*0.5;
	auto dot3 = (oct[4] + oct[5])*0.5;

	return {{dot1, dot2, dot3}};
}

} //trek
