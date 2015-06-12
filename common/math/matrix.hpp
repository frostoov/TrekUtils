#ifndef VECMATH_MATRIX_HPP
#define VECMATH_MATRIX_HPP
#include <cstdint>
#include <vector>
#include <iomanip>
#include "vec.hpp"

namespace vecmath {

template<typename T>
class TMatrix : protected std::vector<T> {
	using vector = std::vector<T>;
	using Matrix = TMatrix<T>;
	using size_t = std::size_t;
  public:

	TMatrix(std::size_t row = 0, std::size_t col = 0)
		: mRow(row), mCol(col) {
		mData.resize(row * col);
	}

	void setSize(std::size_t row, std::size_t col) {
		mRow = row;
		mCol = col;
		mData.clear();
		mData.resize(row * col);
	}

	void fill(T val = 0) {
		for(auto& item : mData)
			item = val;
	}

	void clear() {
		mRow = 0, mCol = 0;
		mData.clear();
	}

	void free() {
		clear();
		mData.shrink_to_fit();
	}

	T& operator()(size_t row, size_t col) {
		return mData.at(row * mCol + col);
	}

	const T& operator()(size_t row, size_t col) const {
		return mData.at(row * mCol + col);
	}

	size_t rows() const {
		return mRow;
	}
	size_t cols() const {
		return mCol;
	}

	size_t size() const {
		return mData.size();
	}
  private:
	vector mData;
	size_t mRow;
	size_t mCol;
};

} //vecmath


#endif // MATRIX

