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
    TCoordSystem3(const Vec3& offset, const Vec3& X, const Vec3& Y, const Vec3& Z)
        : mOffset(offset),
    V1{X}, V2{Y}, V3{Z} { }
    void convertTo(Vec3& vec) const {
        shift(vec);
        rotate(vec);
    }
    void convertTo(Line3& line) const {
        rotate(line.vec());
        convertTo(line.dot());
    }
protected:
    void shift(Vec3& vec) const {
        vec -= mOffset;
    }

    void rotate(Vec3& vec) const {
        vec = {
            vec * V1,
            vec * V2,
            vec * V3,
        };
    }

private:
    Vec3 mOffset;
    Vec3 V1;
    Vec3 V2;
    Vec3 V3;
};

using CoordSystem2 = TCoordSystem2<double>;
using CoordSystem3 = TCoordSystem3<double>;

} //vecmath

#endif // VECMATH_COORDSYSTEM_HPP

