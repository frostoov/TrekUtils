#ifndef VECMATH_POLYGON_HPP
#define VECMATH_POLYGON_HPP

#include <stdexcept>
#include <array>
#include "lines.hpp"
#include "vec.hpp"

namespace vecmath {

template<typename T> class TCoordSystem2;
template<typename T> class TCoordSystem3;

template<typename T>
class TPlane {
	using Plane = TPlane<T>;
	using Line3 = TLine3<T>;
	using Vec3  = TVec3<T>;
public:
	TPlane() {}
	TPlane(const Vec3& dot1, const Vec3& dot2, const Vec3& dot3) {
		mNorm = ((dot2 - dot1)&(dot3 - dot1)).ort();
		mD = -(mNorm * dot1);
	}

	static Line3 getLine(const Plane& plane1, const Plane& plane2) {
		Line3 line;
		auto& dot = line.dot();
		auto& vec = line.vec();
		const auto& norm1 = plane1.norm();
		const auto& norm2 = plane2.norm();

		vec = Vec3(plane1.norm() & plane2.norm()).ort();
		/*Находим точку лежащую на прямой*/
		if(norm1.x() * norm2.y() - norm1.x() * norm2.y()) {
			dot.y() = ( norm2.x() * plane1.d() - norm1.x() * plane2.d() ) /
					   ( norm1.x() * norm2.y()  - norm2.x() * norm1.y());
			if( norm1.x() )
				dot.x() = (-plane1.d() - norm1.y() * dot.y()) / norm1.x();
			else if(norm2.x())
				dot.x() = (-plane2.d() - norm2.y() * dot.y()) / norm2.x();
			dot.z() = 0;
		} else if(norm2.y() * norm1.z() - norm1.y() * norm2.z()) {
			dot.z() = (norm1.y() * plane2.d() - norm2.y() * plane1.d()) /
					   (norm2.y() * norm1.z()  - norm1.y() * norm2.z());
			if(norm1.y())
				dot.y() = (-plane1.d() - norm1.z() * dot.z()) / norm1.y();
			else if(norm2.y())
				dot.y() = (-plane2.d() - norm2.z() * dot.z()) / norm2.y();
			dot.x() = 0;
		} else if(norm2.x() * norm1.z() - norm1.x() * norm2.z()) {
			dot.z() = (norm1.x() * plane2.d() - norm2.x() * plane1.d()) /
					   (norm2.x() * norm1.z()  - norm1.x() * norm2.z());
			if(norm1.x())
				dot.x() = (-plane1.d() - norm1.z() * dot.z()) / norm1.x();
			else if(norm2.x())
				dot.x() = (-plane2.d() - norm2.z() * dot.z()) / norm2.x();
			dot.y() = 0;
		} else
			throw std::runtime_error("TLine3: can't find intersection of two planes");
		return line;
	}

	Vec3& norm() { return mNorm; }
	T& d() { return mD; }

	const Vec3&	norm() const { return mNorm; }
	T d() const { return mD; }

	Vec3 getIntersectionPoint(const Line3& line) {
		auto t = -((mNorm*line.dot()) + mD)/(mNorm*line.vec());
		return (line.vec()*t) + line.dot();
	}
private:
	Vec3 mNorm;
	T mD;
};

template<typename T>
class TQuadrangle2 {
	using Quadrangle2 = TQuadrangle2<T>;
	using Vec2 = TVec2<T>;
  public:
	TQuadrangle2(const Vec2& vtx1,const Vec2& vtx2, const Vec2& vtx3 ,const Vec2& vtx4)
		:mVertices{{vtx1,vtx2,vtx3,vtx4}} {}
	bool checkDot(const Vec2& dot) {
		Vec2 cen( (mVertices[0] + mVertices[1] + mVertices[2] + mVertices[3]) * 0.25 );
		for(short k = 0, l = 1 ; k < 4; ++k,++l) {
			if(l == 4) l = 0;
			Vec2 norm{
				mVertices[l].y() - mVertices[k].y(),
				mVertices[k].x() - mVertices[l].x()
			};
			norm = norm.ort();
			auto C = norm * mVertices[k];
			auto distDot = norm * dot - C;
			auto distCen = norm * cen - C;
			if( std::abs(distCen) < 2) {
				if( std::abs(distDot) > 2)
					return false;
			} else if(distDot * distCen < 0)
				return false;
		}
		return true;
	}

  private:
	std::array<Vec2, 4> mVertices;
};

template<typename T>
class TQuadrangle3 {
	using Quadrangle3 = TQuadrangle3<T>;
	using Quadrangle2 = TQuadrangle2<T>;
	using Plane = TPlane<T>;
	using Line3 = TLine3<T>;
	using Vec2  = TVec2<T>;
	using Vec3  = TVec3<T>;
  public:
	TQuadrangle3(const Vec3& vtx1,const Vec3& vtx2, const Vec3& vtx3 ,const Vec3& vtx4)
		:mVertices{{vtx1,vtx2,vtx3,vtx4}} {}
	Vec3&	operator[](std::size_t i) {
		return mVertices[i];
	}
	bool checkPoint(const Vec3& vec) {
		for(short i = 0,j = 1 ; i < 3; ++i,++j) {
			if(j == 3) j = 0;
			Quadrangle2 tmpRect(
				Vec2{ mVertices[0][i] , mVertices[0][j] },
				Vec2{ mVertices[1][i] , mVertices[1][j] },
				Vec2{ mVertices[2][i] , mVertices[2][j] },
				Vec2{ mVertices[3][i] , mVertices[3][j] }
			);
			Vec2 tmpDot(vec[i],vec[j]);
			if(!tmpRect.checkDot(tmpDot))
				return false;
		}
		return true;
	}

	bool checkIntersection(const Line3& line, Vec3& point) {
		point = getPlane().intersectionPoint(line);
		return checkPoint(point);
	}

	Plane getPlane() {
		Plane plane;
		plane.norm() = ((mVertices[1] - mVertices[0])&(mVertices[2] - mVertices[0])).ort();
		plane.d()    = -(plane.norm()*mVertices[0]);
		return plane;
	}
  private:
	std::array<Vec3, 4> mVertices;
};

template<typename T>
class TOctahedron {
	using Octahedron = TOctahedron<T>;
	using Vec3 = TVec3<T>;
	using Line3 = TLine3<T>;
  public:
	TOctahedron() {}
	TOctahedron(const Vec3& vtx1, const Vec3& vtx2, const Vec3& vtx3, const Vec3& vtx4,
			   const Vec3& vtx5, const Vec3& vtx6, const Vec3& vtx7, const Vec3& vtx8)
		: mVertices{{vtx1, vtx2, vtx3, vtx4, vtx5, vtx6, vtx7, vtx8}} {}
	std::vector<Vec3> checkIntersection(const Line3& line) const;
	Vec3& operator[](size_t i) { return mVertices.at(i); }
	const Vec3& operator[](size_t i) const { return mVertices.at(i); }
	std::array<Vec3, 8>& vertices() { return mVertices; }
	const std::array<Vec3, 8>& vertices() const { return mVertices; }
  private:
	std::array<Vec3, 8> mVertices;
};

}

#endif // VECMATH_POLYGON_HPP
