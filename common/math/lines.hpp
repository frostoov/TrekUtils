#ifndef VECMATH_LINES_HPP
#define VECMATH_LINES_HPP

#include <vector>
#include <limits>
#include "vec.hpp"

namespace vecmath {

template<typename T>
class TLine2 {
	using Line2 = TLine2<T>;
	using Vec2  = TVec2 <T>;
  public:
	TLine2()
		: koefK(0), koefB(0) {}

	TLine2(const Vec2& dot1, const Vec2& dot2) {
		auto dirVec = dot1 - dot2;
		if(dirVec.x() != 0)
			koefK = dirVec.y() / dirVec.x();
		else {
			if(std::numeric_limits<T>::has_infinity)
				koefK = std::numeric_limits<T>::infinity();
			else
				koefK = std::numeric_limits<T>::max();
		}
		koefB = dot1.y() - koefK * dot1.x();

	}

	T& k() { return koefK; }
	T& b() { return koefB; }

	T k() const { return koefK; }
	T b() const { return koefB; }

	static Vec2 getIntersection(const Line2& line1, const Line2& line2) {
		if( line1.k() == line2.k() )
			throw std::runtime_error("Vec2::getIntersection: Lines are parallel");
		if(line1.b() != line2.b()) {
			auto x = ( line1.k() - line2.k() ) / (line2.b() - line1.b());
			auto y = line1.k() * x + line1.b();
			return {x, y};
		} else
			return {0, 0};
	}

	void checkRectangle(T x0, T x1, T y0, T y1, std::vector<Vec2>& points) const {
		if(!koefK) {
			if(koefB >= y0 && koefB <= y1) {
				points.push_back( {x0, koefB} );
				points.push_back( {x1, koefB} );
			}
		} else {
			T lx0 = (y0 - koefB) / koefK;
			T lx1 = (y1 - koefB) / koefK;
			T ly0 = koefK * x0 + koefB;
			T ly1 = koefK * x1 + koefB;

			if(ly0 >= y0 && ly0 <= y1)
				points.push_back({x0, ly0});
			if(ly1 >= y0 && ly1 <= y1)
				points.push_back({x1, ly1});
			if(lx0 >= x0 && lx0 <= x1)
				points.push_back({lx0, y0});
			if(lx1 >= x0 && lx1 <= x1)
				points.push_back({lx1, y1});
		}
	}

	std::vector<Vec2> checkRectangle(T x0, T x1, T y0, T y1) const {
		std::vector<Vec2> points;
		checkRectangle(x0, x1, y0, y1, points);
		return points;
	}
  private:
	T koefK;
	T koefB;
};

template<typename T>
class TLine3 {
	using Line3 = TLine3<T>;
	using Vec3  = TVec3<T>;
  public:
	TLine3() {}
	TLine3(const Vec3& dot1, const Vec3& dot2) {
		if(dot1.abs() < dot2.abs()) {
			mVec = (dot2 - dot1).ort();
			mDot = dot1;
		} else {
			mVec = (dot1 - dot2).ort();
			mDot = dot2;
		}
	}

	Vec3& dot() { return mDot; }
	Vec3& vec() { return mVec; }

	const Vec3& dot() const { return mDot; }
	const Vec3& vec() const { return mVec; }
	static Line3 getLineFromProjections(Line3 ln1, Line3 ln2);
	void rotateX(double ang) {
		mDot.rotateX(ang);
		mVec.rotateX(ang);
	}
	void rotateY(T ang) {
		mDot.rotateY(ang);
		mVec.rotateY(ang);
	}
	void rotateZ(T ang) {
		mDot.rotateZ(ang);
		mVec.rotateZ(ang);
	}
	void rotate(const Vec3& vec, T ang) {
		mDot.rotate(vec, ang);
		mVec.rotate(vec, ang);
	}
  private:
	Vec3 mDot;
	Vec3 mVec;
};

using Line2 = TLine2<double>;
using Line3 = TLine3<double>;

} //vecmath


#endif // VECMATH_LINES_HPP

