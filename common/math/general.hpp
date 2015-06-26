#ifndef MATH_GENERAL_HPP
#define MATH_GENERAL_HPP

namespace vecmath {

static constexpr double PI    = 3.14159265358979323846;
static constexpr double E     = 2.71828182845904523536;

template<typename T>
const T toDeg(const T& angle) {
	return angle * 180. / PI;
}

template<typename T>
const T toRad(const T& angle) {
	return angle * PI / 180.;
}

} //vecmath

#endif // MATH_GENERAL_HPP

