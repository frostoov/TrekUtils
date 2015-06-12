#ifndef VECMATH_COORDSYSTEM_HPP
#define VECMATH_COORDSYSTEM_HPP

#include "lines.hpp"

namespace vecmath {

template <typename T>
class TCoordSystem2 {
	using Vec2  = TVec2<T>;
	using Line2 = TLine2<T>;
  public:
	TCoordSystem2(Vec2 _offset, T _ang)
		: mOffset(_offset), mAng(_ang) {}
	void convert(Vec2& vec) const {
		/*Перенос*/
		vec -= mOffset;
		vec.rotate(mAng);
	}
	void convert(Line2& line) const {
		convert(line.dot());
		convert(line.vec());
	}
  private:
	Vec2 mOffset;
	T	 mAng;
};

template <typename T>
class TCoordSystem3 {
	using Vec3 = TVec3<T>;
  public:
	TCoordSystem3()
		: V1{0, 0, 0}, V2{0, 0, 0}, V3{0, 0, 0} {}
	TCoordSystem3(const Vec3& offset, const Vec3& X, /*const Vec3 &Y,*/ const Vec3& Z)
		: mOffset(offset) {
		const Vec3 x = Vec3(1, 0, 0);
		const Vec3 z = Vec3(0, 0, 1);
		//Line of nodes
		auto n = (z & Z).ort();
		//Euler angles
		auto mAlpha = X.angle(n), mGama = x.angle(n), mBeta = z.angle(Z);

		auto sinA = std::sin(mAlpha), sinB = std::sin(mBeta), sinY = std::sin(mGama);
		auto cosA = std::cos(mAlpha), cosB = std::cos(mBeta), cosY = std::cos(mGama);

		V1[0] = cosA * cosY - sinA * cosB * sinY, V1[1] = -cosA * sinY - sinA * cosB * cosY, V1[2] = sinA * sinB;
		V2[0] = sinA * cosY + cosA * cosB * sinY, V2[1] = -sinA * sinY + cosA * cosB * cosY, V2[2] = -cosA * sinB;
		V3[0] = sinB * sinY,					V3[1] = sinB * cosY,				     V3[2] = cosB;
	}
	void convertTo(Vec3& vec) const {
		vec -= mOffset;
		vec = {
			vec.x()* V1[0] + vec.y()* V1[1] + vec.z()* V1[2],
			vec.x()* V2[0] + vec.y()* V2[1] + vec.z()* V2[2],
			vec.x()* V3[0] + vec.y()* V3[1] + vec.z()* V3[2],
		};
	}
	void convertTo(Line3& line) const {
		convertTo(line.vec());
		convertTo(line.dot());
	}

  private:
	Vec3 mOffset;
	T V1[3];
	T V2[3];
	T V3[3];
};

using CoordSystem2 = TCoordSystem2<double>;
using CoordSystem3 = TCoordSystem3<double>;

} //vecmath

#endif // VECMATH_COORDSYSTEM_HPP

