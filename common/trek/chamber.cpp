#include "chamber.hpp"

namespace trek {

Chamber::Chamber(const ChamberPosition& position)
	: mPlane(position.plane), mGroup(position.group),
	  mIsHit(false), mHasTrack(false), mIsSelected(false) {
	mOct = getOctahedron(position.vertices.at(0),
						 position.vertices.at(1),
						 position.vertices.at(2));
}

void Chamber::loadVertices(std::vector<float>& vertex) const
{
	for(auto& octVertex : mOct.vertices())
		for(int i = 0; i < 3; ++i)
			vertex.push_back(octVertex[i]);
}

void Chamber::loadPlgColors(std::vector<float>& colors) const
{
	for(int k = 0 ; k < 8 ; ++k) {
		for(int i = 0; i < 3; ++i) {
			if(isHit()) {
				if(!i) colors.push_back(0.8f);
				else   colors.push_back(0.1f);
			} else
				colors.push_back(0.5f);
		}
		colors.push_back(0.7f);
	}
}

void Chamber::loadLineColors(std::vector<float>& colors) const
{
	for(short k = 0 ; k < 8 ; ++k) {
		for(short i = 0; i < 3; ++i) {
			if(isSelected()) {
				if(i == 2)
					colors.push_back(0.8f);
				else
					colors.push_back(0.1f);
			} else
				colors.push_back(0.0);
		}
		colors.push_back(1.0);
	}
}

void Chamber::loadPlgFace(std::vector<uint>& face, uint start) const
{
	for(char i = 0 ; i < 4 ; ++ i)
		face.push_back(start + i);
	for(char i = 4 ; i < 8; ++i)
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

void Chamber::loadLineFace(std::vector<uint>& face, uint start) const
{
	for(char  k = 0; k <= 4; k += 4) {
		for(char i = k ; i < k + 3; ++i) {
			face.push_back(start + i);
			face.push_back(start + i + 1);
		}
		face.push_back(start + 3 + k);
		face.push_back(start + k);
	}
	for(char i = 0 ; i < 4 ; ++i) {
		face.push_back(start + i);
		face.push_back(start + i + 4);
	}
}

Chamber::Plane Chamber::getTrackPlane()
{
	if(mHasTrack == false)
		throw std::runtime_error("Chamber: getTrackPlane: no track");
	auto dot1 = (mOct[0] + mOct[1]) * 0.5;
	auto dot2 = (mOct[2] + mOct[3]) * 0.5;
	auto dot3 = (mOct[4] + mOct[5]) * 0.5;

	auto p13 = dot3 - dot1;
	auto p12 = dot2 - dot1;
	auto wVec = (p13&p12).ort();

	auto pt1 = dot1 + wVec*mTrack.line.b();
	auto pt2 = dot2 + wVec*(mTrack.line.b() + mTrack.line.k() * chamberHeight);
	auto pt3 = dot3 + wVec*mTrack.line.b();

	return Plane(pt1, pt2, pt3);
}

Chamber::Octahedron Chamber::getOctahedron(const Vec3& dot1, const Vec3& dot2, const Vec3& dot3) const
{
	Octahedron ret;
	const int chamWidth = 250;

	/*Вспомогательные векторы*/
	const auto p13 = dot3 - dot1;
	const auto p12 = dot2 - dot1;

	auto widthVec = p12&p13;
	widthVec = widthVec.ort()*chamWidth;

	/*Развертывание первого полигона*/
	ret[0] = dot1 + widthVec;
	ret[1] = dot1 - widthVec;
	ret[2] = ret[1] + p12;
	ret[3] = ret[0] + p12;

	/*Противоположный полигон*/
	for(char i = 4; i < 8; ++i)
		ret[i] = ret[i - 4] + p13;

	return ret;
}

} //trek
