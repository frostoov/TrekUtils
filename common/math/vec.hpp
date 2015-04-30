#ifndef VECMATH_VEC3_HPP
#define VECMATH_VEC3_HPP

#include <cmath>
#include <stdexcept>

namespace vecmath {

template<typename T>
/**
 * @class TVec2
 * @author frostoov
 * @date 03/18/15
 * @file vec.hpp
 * @brief Вектор в двухмерном пространстве
 */
class TVec2 {
	using Vec2 = TVec2<T>;
  public:
	TVec2(T x = 0,T y = 0)
		:mData{x,y} {}

	/**
	 * @brief Оператор сравнения
	 * @param vec Вектор, с которым производится сравнение
	 * @return true - векторы равны, false - если векторы не равны
	 */
	bool operator ==(const Vec2& vec) const {
		return (mData[0] == vec.mData[0] &&
		        mData[1] == vec.mData[1]);
	}
	/**
	 * @brief
	 * @param num
	 * @return
	 */
	const Vec2 operator*(T num) const {
		return {mData[0]* num,
		        mData[1]* num};
	}
	/**
	 * @brief Сложение векторов
	 * @param vec Вектор, который прибавляется к данному
	 * @return Сумма векторов
	 */
	const Vec2 operator+(const Vec2& vec) const {
		return {mData[0] + vec.mData[0],
		        mData[1] + vec.mData[1]};
	}
	/**
	 * @brief Разница векторов
	 * @param vec Вектор, который отнимается от данного
	 * @return Разниц векторов
	 */
	const Vec2 operator-(const Vec2& vec) const {
		return {mData[0] - vec.mData[0],
		        mData[1] - vec.mData[1]};
	}
	/**
	 * @brief Скалярное умножеие векторов
	 * @param vec Вектор, на который умножается данный
	 * @return Скалярное произведение векторов
	 */
	const T operator*(const Vec2& vec) const {
		return	mData[0] * vec.mData[0] +
		        mData[1] * vec.mData[1];
	}
	/**
	 * @brief Бинарный плюс
	 * @return Возрщает себя
	 */
	Vec2 operator +() const {
		return *this;
	}
	/**
	 * @brief Бинарный минус
	 * @return Возвращает обраный вектор
	 */
	Vec2 operator -() const {
		return {-mData[0], -mData[1]};
	}
	/**
	 * @brief Сложение с данным вектором
	 * @param vec Вектор, который прибавляется к данному
	 */
	void operator+=(const Vec2& vec) {
		mData[0] += vec.mData[0];
		mData[1] += vec.mData[1];
	}
	/**
	 * @brief Вычитание из данного вектора
	 * @param vec Вектор, который отнимается от данного
	 */
	void operator-=(const Vec2& vec) {
		mData[0] -= vec.mData[0];
		mData[1] -= vec.mData[1];
	}
	/**
	 * @brief Вычисление орта данного вектора
	 * @return Орт данного вектора
	 */
	Vec2 ort() const {
		auto len = this->abs();
		if(len)
			return {mData[0]/len, mData[1]/len};
		else
			return {0,0};
	}

	/**
	 * @brief Вычисление длины данного вектора
	 * @return Длина данного вектора
	 */
	T abs() const {
		return std::sqrt(std::pow(mData[0], 2) +
		                 std::pow(mData[1], 2) );
	}

	/**
	 * @brief Координата X
	 * @return Ссылка на координату X
	 */
	T& x() {
		return mData[0];
	}
	/**
	 * @brief Координата Y
	 * @return Ссылка на координату Y
	 */
	T& y() {
		return mData[1];
	}

	/**
	 * @brief Координата X
	 * @return Копия координаты X
	 */
	T x() const {
		return mData[0];
	}
	/**
	 * @brief Координата Y
	 * @return Копия координаты Y
	 */
	T y() const {
		return mData[1];
	}

	T& operator[](int i) {
		if(i < 2)
			return mData[i];
		else throw std::out_of_range("Vec2: out of range");
	}
	T operator[](int i) const {
		if(i < 2)
			return mData[i];
		else throw std::out_of_range("Vec2: out of range");
	}

	/**
	 * @brief Вращение вектора
	 * @param ang Угол, на которое производится вращение
	 */
	void rotate(T ang) {
		T s = sin(ang);
		T c = cos(ang);
		*this = {
			x()* c - y()* s,
			x()* s + y()* c,
		};
	}
  private:
	T mData[2];
};


template< typename T>
class TVec3 {
	using Vec3 = TVec3<T>;
  public:
	TVec3(T x = 0,T y = 0, T z = 0)
		: mData{x,y,z} {}

	TVec3(const T* data)
		: mData{data[0], data[1], data[2]} {}

	const T operator*(const Vec3& vec) const {
		return  mData[0]*vec.mData[0] +
		        mData[1]*vec.mData[1] +
		        mData[2]*vec.mData[2];
	}

	const Vec3 operator*(const T& num) const {
		return {mData[0]* num,
		        mData[1]* num,
		        mData[2]* num};
	}
	const Vec3 operator/(const T& num) const {
		return {mData[0]/num,
		        mData[1]/num,
		        mData[2]/num};
	}
	const Vec3 operator&(const Vec3& vec) const {
		return {
			mData[1]* vec.mData[2] - mData[2]* vec.mData[1],
			mData[2]* vec.mData[0] - mData[0]* vec.mData[2],
			mData[0]* vec.mData[1] - mData[1]* vec.mData[0],
		};
	}
	const Vec3 operator+(const Vec3& vec) const {
		return {mData[0] + vec.mData[0],
		        mData[1] + vec.mData[1],
		        mData[2] + vec.mData[2]};
	}
	const Vec3 operator-(const Vec3& vec) const {
		return {mData[0] - vec.mData[0],
		        mData[1] - vec.mData[1],
		        mData[2] - vec.mData[2]};
	}
	const Vec3 operator+() const {
		return *this;
	}
	const Vec3 operator-() const {
		return {-mData[0], -mData[1], -mData[2]};
	}
	void operator+=(const Vec3& vec) {
		mData[0] += vec.mData[0];
		mData[1] += vec.mData[1];
		mData[2] += vec.mData[2];
	}
	void operator-=(const Vec3& vec) {
		mData[0] -= vec.mData[0];
		mData[1] -= vec.mData[1];
		mData[2] -= vec.mData[2];
	}
	const Vec3 ort() const {
		auto len = abs();
		if(len) {
			len = 1/len;
			return {mData[0]* len, mData[1]* len, mData[2]* len};
		} else
			return {0,0,0};
	}
	const T abs() const {
		return std::sqrt( std::pow(mData[0], 2) +
		                  std::pow(mData[1], 2) +
		                  std::pow(mData[2], 2) );
	}

	T range(const Vec3& vec) const {
		return (*this - vec).abs();
	}

	const T angle(const Vec3& vec) const {
		return std::acos((*this*vec)/(this->abs()*vec.abs()) );
	}
	T& x() {
		return mData[0];
	}
	T& y() {
		return mData[1];
	}
	T& z() {
		return mData[2];
	}

	T x() const {
		return mData[0];
	}
	T y() const {
		return mData[1];
	}
	T z() const {
		return mData[2];
	}

	T* data() {return mData;}
	const T* data() const {return mData;}

	T& operator[](unsigned int i) {
		if(i < 3)
			return mData[i];
		else throw std::out_of_range("Vec2: out of range");
	}
	T  operator[](unsigned int i) const {
		if(i < 3)
			return mData[i];
		else throw std::out_of_range("Vec2: out of range");
	}

	void rotateX(T ang) {
		auto c = std::cos(ang),s = std::sin(ang);
		auto Y = y(),Z = z();
		mData[1] = Y * c - Z * s;
		mData[2] = Y * s + Z * c;
	}
	void rotateY(T ang) {
		auto c = std::cos(ang), s = std::sin(ang);
		auto X = x(),Z = z();
		mData[0] = X * c + Z * s;
		mData[2] = Z * c - X * s;
	}
	void rotateZ(T ang) {
		auto c = std::cos(ang),s = std::sin(ang);
		auto X = x(),Y = y();
		mData[0] = X * c - Y * s;
		mData[1] = X * s + Y * c;
	}
	void rotate (const Vec3& vec,T ang) {
		auto v = vec.ort();
		auto c = std::cos(ang);
		auto s = std::sin(ang);
		auto d = 1 - c;
		double V1[] = {c + d*v.x()* v.x(),       d*v.x()* v.y() - s*v.z(), d*v.x()* v.z() + s*v.y()};
		double V2[] = {d*v.x()* v.y() + s*v.z(), c + d*v.y()* v.y(),	     d*v.y()* v.z() + s*v.x()};
		double V3[] = {d*v.x()* v.z() + s*v.y(), c*v.y()* v.z() + s*v.x(), c + d *v.z()* v.z()     };
		*this = {
			V1[0]* x() + V1[1]* y() + V1[2]* z(),
			V2[0]* x() + V2[1]* y() + V2[2]* z(),
			V3[0]* x() + V3[1]* y() + V3[2]* z(),
		};
	}

	T* begin() {return mData;}
	T* end()   {return mData + 3;}

	const T* begin() const {return mData;}
	const T* end()   const {return mData + 3;}

  private:
	T mData[3];
};

using Vec2 = TVec2<double>;
using Vec3 = TVec3<double>;

static constexpr double PI    = 3.14159265358979323846;
static constexpr double todeg = 180./PI;

}
#endif // VECMATH_VEC3_HPP
