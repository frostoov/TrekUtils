#ifndef MATRIXEVENTHANDLER_HPP
#define MATRIXEVENTHANDLER_HPP

#include "tdcdata/event.hpp"
#include "trek/trekhandler.hpp"
#include "math/matrix.hpp"

class MatrixHandler : public tdcdata::AbstractEventHandler {
	using Matrix       = vecmath::TMatrix<uintmax_t>;
	using DMatrix	   = vecmath::TMatrix<double>;
	using MatricesMap  = std::unordered_map<std::uintmax_t, Matrix>;
	using DMatricesMap = std::unordered_map<std::uintmax_t, DMatrix>;
  public:
	MatrixHandler(const trek::ChamberConfig& config, uint32_t pedestal, double speed);

	void handleEvent(const tdcdata::TUEvent& rawEvent) override;
	void flush() override;
  protected:
	void outputMatriciesMap();
	void outputMatriciesMap(const MatricesMap& matrices, const std::string& pattern);
	template<typename T>
	void outputMatrix(const vecmath::TMatrix<T>& mat, const std::string& pattern);
	template<typename T>
	void outputMatrix(std::ostream& str, const vecmath::TMatrix<T>& matrix);
  private:
	Matrix		mMatrixN;
	MatricesMap	mMatricesT;
	DMatricesMap mMatricesDev;
	DMatricesMap mMatricesDeltaB;
	DMatricesMap mMatricesDeltaA;

	trek::TrekHandler mTrekHandler;

	static const std::size_t rows = 161;
	static const std::size_t cols = 321;
};

#endif // MATRIXEVENTHANDLER_HPP
