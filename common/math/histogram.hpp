#ifndef VECMATH_HISTOGRAM_HPP
#define VECMATH_HISTOGRAM_HPP

#include <vector>
#include <cstdint>
#include <cstddef>
#include <stdexcept>

namespace vecmath {

template<typename T>
class Histogram {
	using vector = std::vector<uintmax_t>;
	using size_type = vector::size_type;
  public:
	using cell = std::pair<T, uintmax_t>;

	Histogram(T bottom = 0, T up = 0, T bin = 0)
		: mBin(0), mUp(0), mBottom(0) {
		setParameters(bottom,up,bin);
	}
	void setParameters(T bottom, T up, T bin) {
		buffer.clear();
		if(bottom > up)
			throw std::runtime_error("Invalid histogram parametres");
		mBottom	= bottom;
		mUp		= up;
		mBin	= bin;
		buffer.resize((mUp - mBottom)/mBin + 1);
	}
	void expand(T newUp) {
		mUp = newUp;
		buffer.resize((mUp - mBottom)/mBin + 1);
	}
	void addValue(const T val) {
		buffer.at( (val - mBottom)/mBin )++;
	}
	uintmax_t value(size_t i) {
		return buffer.at(i);
	}
	T range(size_t i) {
		return mBottom + i * mBin;
	}
	cell operator [](size_t n) {
		return {mBottom + n * mBin, buffer.at(n)};
	}
	uintmax_t maxValue() {
		uintmax_t max = 0;
		for(auto val : buffer)
			if(max < val)
				max = val;
		return max;
	}

	T maxValueRange() {
		uintmax_t	max = 0;
		size_type	rangeIndex = 0;
		for(size_t i = 0; i < buffer.size(); ++i)
			if(max < buffer.at(i)) {
				max = buffer.at(i);
				rangeIndex = i;
			}
		return range(rangeIndex);
	}
	T getBottom() const	{
		return mBottom;
	}
	T getUp()	  const	{
		return mUp;
	}
	T getBin()	  const	{
		return mBin;
	}

	size_type size() {
		return buffer.size();
	}
	void clear()	{
		buffer.clear();
	}
	void free()	{
		buffer.clear();
		buffer.shrink_to_fit();
	}
  private:
	vector buffer;
	T mBin;
	T mUp;
	T mBottom;
};

} // vecmath

#endif // VECMATH_HISTOGRAM_HPP
