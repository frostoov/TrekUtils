#include <fstream>
#include "matrixhandler.hpp"

using trek::ChamberConfig;
using std::acos;
using tdcdata::TUEvent;
using tdcdata::UIntVector;
using vecmath::PI;
using vecmath::toDeg;
using std::to_string;
using std::atan;

MatrixHandler::MatrixHandler(const trek::ChamberConfig& config, const std::string& dirPath)
	: mMatrixN(rows, cols), mTrekHandler(config), mDirPath(dirPath) {}

void MatrixHandler::handleEvent(const TUEvent& rawEvent) {
	mTrekHandler.loadEvent(rawEvent);
	struct TPoint3D { double x, y, z; };

	auto eventChecker = [&](const UIntVector & data, uintmax_t cham)->bool {
		uint8_t wireFlag = 0;
		for(auto word : data)
			if( TUEvent::getChamber(word) == cham ) {
				auto wire = TUEvent::getWire(word);
				wireFlag |= 1 << wire;
				if(wireFlag == 15)
					return true;
			}
		return false;
	};

	auto& uraganEvent = rawEvent.getUraganEvent();

	TPoint3D P1 { uraganEvent.chp0[0], uraganEvent.chp0[1], uraganEvent.chp0[2], };
	TPoint3D P2 { uraganEvent.chp1[0], uraganEvent.chp1[1], uraganEvent.chp1[2], };

	auto dx = P2.x - P1.x;
	auto dy = P2.y - P1.y;
	auto dz = P2.z - P1.z;
	char iz = 1;

	double r = std::sqrt(dx * dx + dy * dy + dz * dz);
	if(r == 0 || dz == 0)
		return;

	if(dz < 0) {
		iz = -1;
		dx = -dx, dy = -dy, dz = -dz;
	}
	dx /= r;
	dy /= r;
	dz /= r;
	auto ThetaG = toDeg( std::acos(dz) ); // зенитный угол

	if(ThetaG >= 0. && ThetaG < 20.) {
		//Найдём пересечение треком горизонтальной плоскости
		double z = 10825;
		double t = (z - P1.z) * iz / dz;
		double Dx = P1.x - 5300 + iz * dx * t; 	// 3500  - левый  край триггируемой области УРАГАНА
		double y = P1.y - 12250 + iz * dy * t; 	// 12250 - нижний край триггируемой области УРАГАНА
		double x = Dx + y * sin(0.25 * PI / 180);
		size_t ix = (x / 22. + 0) + 0.5;		// 22    - размер ячейки
		size_t iy = (y / 22. + 0) + 0.5;
		if(ix < cols && iy < rows) {
			++mMatrixN(iy, ix);
			auto triggChambers = rawEvent.getTriggeredChambers();
			for(auto cham : triggChambers) {
				if(mMatricesT.count(cham) == 0)
					mMatricesT[cham] = Matrix(rows, cols);
				auto& tMatrix = mMatricesT.at(cham);
				if(eventChecker(rawEvent.getTrekEventRaw(), cham) &&
						iy < tMatrix.rows() && ix < tMatrix.cols())
					++tMatrix(iy, ix);
			}
			for(const auto& chamberPair : mTrekHandler.getChambers()) {
				auto  chamNumber  = chamberPair.first;
				auto& chamber     = chamberPair.second;
				if(chamber.hasTrack()) {
					auto uragan = chamber.getUraganProjection(rawEvent.getUraganEvent().chp0,
								  rawEvent.getUraganEvent().chp1);
					auto track = chamber.getTrackDescription();
					auto trackAngle  = toDeg( std::atan(track.line.k()) );
					auto uraganAngle = toDeg( std::atan(uragan.k()) );
					auto deltaB      = track.line.b() - uragan.b();
					auto deltaA      = trackAngle - uraganAngle;
					if(mMatricesDev.count(chamNumber) == 0)
						mMatricesDev[chamNumber] = DMatrix(rows, cols);
					mMatricesDev.at(chamNumber)(iy, ix) += track.deviation;
					if(mMatricesDeltaA.count(chamNumber) == 0)
						mMatricesDeltaA[chamNumber] = DMatrix(rows, cols);
					mMatricesDeltaA.at(chamNumber)(iy, ix) += deltaA;
					if(mMatricesDeltaB.count(chamNumber) == 0)
						mMatricesDeltaB[chamNumber] = DMatrix(rows, cols);
					mMatricesDeltaB.at(chamNumber)(iy, ix) += deltaB;
				}
			}
		}
	}
}

void MatrixHandler::flush() {
	outputMatriciesMap();
	mMatrixN.fill();
	mMatricesT.clear();
}

void MatrixHandler::outputMatriciesMap() {
	outputMatrix(mMatrixN, "matrixN");
	outputMatriciesMap(mMatricesT, "matrixT");
	DMatrix tempMatrix(rows, cols);
	for(const auto& matrixPair : mMatricesT) {
		auto& matrix = matrixPair.second;
		auto cham = matrixPair.first;
		for (size_t i = 0; i < rows; ++i)
			for (size_t j = 0; j < cols; ++j)
				if(mMatrixN(i, j) != 0)
					tempMatrix(i, j) =  static_cast<double>(matrix(i, j)) / mMatrixN(i, j);
		outputMatrix(tempMatrix, "matrixE" + to_string(cham + 1));
	}
	tempMatrix.fill(0);
	for(const auto& matrixPair : mMatricesDev) {
		auto& matrix = matrixPair.second;
		auto cham = matrixPair.first;
		for (size_t i = 0; i < rows; ++i)
			for (size_t j = 0; j < cols; ++j)
				if(mMatrixN(i, j) != 0)
					tempMatrix(i, j) =  static_cast<double>(matrix(i, j)) / mMatrixN(i, j);
		outputMatrix(tempMatrix, "matrixDev" + to_string(cham + 1));
	}
	tempMatrix.fill(0);
	for(const auto& matrixPair : mMatricesDeltaA) {
		auto& matrix = matrixPair.second;
		auto cham = matrixPair.first;
		for (size_t i = 0; i < rows; ++i)
			for (size_t j = 0; j < cols; ++j)
				if(mMatrixN(i, j) != 0)
					tempMatrix(i, j) =  static_cast<double>(matrix(i, j)) / mMatrixN(i, j);
		outputMatrix(tempMatrix, "matrixDeltaA" + to_string(cham + 1));
	}
	tempMatrix.fill(0);
	for(const auto& matrixPair : mMatricesDeltaB) {
		auto& matrix = matrixPair.second;
		auto cham = matrixPair.first;
		for (size_t i = 0; i < rows; ++i)
			for (size_t j = 0; j < cols; ++j)
				if(mMatrixN(i, j) != 0)
					tempMatrix(i, j) =  static_cast<double>(matrix(i, j)) / mMatrixN(i, j);
		outputMatrix(tempMatrix, "matrixDeltaB" + to_string(cham + 1));
	}
}

void MatrixHandler::outputMatriciesMap(const MatricesMap& matrices, const std::string& pattern) {
	for(const auto& mat : matrices)
		outputMatrix(mat.second, pattern + to_string(mat.first + 1));
}


template<typename T>
void MatrixHandler::outputMatrix(const vecmath::TMatrix<T>& mat, const std::string& pattern) {
	std::ofstream stream;
	stream.exceptions(std::ofstream::badbit | std::ofstream::failbit);
	stream.open(mDirPath + "/" + pattern + ".dat");
	outputMatrix(stream, mat);
}

template<typename T>
void MatrixHandler::outputMatrix(std::ostream& str, const vecmath::TMatrix<T>& matrix) {
	str << "!Xmin=0.,Xmax=" << matrix.cols()
		<< ".,Ymin=0.,Ymax=" << matrix.rows() << ".\n";
	str << std::fixed;
	for(size_t iy = 0; iy < matrix.rows(); iy++) {
		for(size_t ix = 0; ix < matrix.cols(); ix++) {
			if(matrix(iy, ix) != 0)
				str << std::setprecision(6) << matrix(iy, ix) << '\t';
			else
				str << '-' << '\t';
		}
		str << '\n';
	}
}
