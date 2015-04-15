#ifndef MATRIXEVENTHANDLER_HPP
#define MATRIXEVENTHANDLER_HPP

#include "tdcdata/event.hpp"
#include "math/matrix.hpp"

class MatrixHandler : public tdcdata::AbstractEventHandler {
	using Matrix      = vecmath::TMatrix<uintmax_t>;
	using DMatrix	  = vecmath::TMatrix<double>;
	using MatricesMap = std::unordered_map<std::uintmax_t, Matrix>;

  public:
	MatrixHandler();
	~MatrixHandler();

	void handleEvent(const tdcdata::TUEvent& event) override;
	void flush() override;
  protected:
	void outputMatriciesMap();
	void outputMatriciesMap(const MatricesMap& matrices, const std::string& pattern);
	template<typename T>
	void outputMatrix(const vecmath::TMatrix<T>& mat, const std::string& pattern);
	template<typename T>
	void outputMatrix(std::ostream& str, const vecmath::TMatrix<T>& matrix);
  private:
	Matrix			mMatrixN;
	MatricesMap	mMatricesT;
	uintmax_t goodEvents;
	uintmax_t badEvents;
	uintmax_t emptyEvents;
	uintmax_t fullEmpty;

	static const std::size_t rows = 161;
	static const std::size_t cols = 161;
};

#endif // MATRIXEVENTHANDLER_HPP
