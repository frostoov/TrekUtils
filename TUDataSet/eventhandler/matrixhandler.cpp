#include <iostream>
#include <fstream>
#include "matrixhandler.hpp"

using std::acos;
using tdcdata::TUEvent;
using tdcdata::UIntVector;
using vecmath::PI;
using vecmath::todeg;
using std::cout;
using std::endl;
using std::to_string;

MatrixHandler::MatrixHandler()
	: mMatrixN(rows, cols) {}

void MatrixHandler::handleEvent(const TUEvent& event) {
	struct TPoint3D {
		double x, y, z;
	};

	auto eventChecker = [&](const UIntVector &data, uintmax_t cham)->bool {
		uint8_t wireFlag = 0;
		for(auto word : data)
			if( TUEvent::getChamber(word) == cham ) {
				auto wire = TUEvent::getWire(word);
				wireFlag |= 1<<wire;
				if(wireFlag == 15)
					return true;
			}
		return false;
	};

	auto& uraganEvent = event.getUraganEvent();

	TPoint3D P1 { uraganEvent.chp0[0], uraganEvent.chp0[1], uraganEvent.chp0[2], };
	TPoint3D P2 { uraganEvent.chp1[0], uraganEvent.chp1[1], uraganEvent.chp1[2], };

	auto dx = P2.x - P1.x;
	auto dy = P2.y - P1.y;
	auto dz = P2.z - P1.z;
	char iz = 1;

	double r = std::sqrt(dx*dx+dy*dy+dz*dz);
	if(r == 0 || dz == 0)
		return;

	if(dz < 0) {
		iz = -1;
		dx =-dx,dy =-dy,dz =-dz;
	}
	dx /= r;
	dy /= r;
	dz /= r;
	auto ThetaG = std::acos(dz)*todeg;   // зенитный угол

	if(ThetaG >= 0. && ThetaG < 20.) {
		//Найдём пересечение треком горизонтальной плоскости
		double z = 10825;
		double t = (z-P1.z)*iz/dz;
		double Dx = P1.x-5300+iz*dx*t; 	// 3500  - левый  край триггируемой области УРАГАНА
		double y = P1.y-12250+iz*dy*t; 	// 12250 - нижний край триггируемой области УРАГАНА
		double x = Dx + y*sin(0.25*PI/180);
		size_t ix = (x/22.+0)+0.5;		// 22    - размер ячейки
		size_t iy = (y/22.+0)+0.5;
		if(ix < mMatrixN.cols() && iy < mMatrixN.rows()) {
			++mMatrixN(iy, ix);
			auto triggChambers = event.getTriggeredChambers();
			for(auto cham : triggChambers) {
				if(mMatricesT.count(cham) == 0)
					mMatricesT[cham] = Matrix(rows, cols);
				auto& tMatrix = mMatricesT.at(cham);
				if(eventChecker(event.getTrekEventRaw(), cham) &&
				        iy < tMatrix.rows() && ix < tMatrix.cols())
					++tMatrix(iy, ix);
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
	outputMatriciesMap(mMatricesT,"matrixT");
	DMatrix matrixE(rows, cols);
	for(const auto& matrixTPair : mMatricesT) {
		auto& matrixT = matrixTPair.second;
		auto cham = matrixTPair.first;
		for (size_t i = 0; i < matrixE.rows(); ++i)
			for (size_t j = 0; j < matrixE.cols(); ++j)
				if(mMatrixN(i,j) != 0)
					matrixE(i, j) =  static_cast<double>(matrixT(i, j)) / mMatrixN(i, j);
		outputMatrix(matrixE, "matrixE" + to_string(cham+1));
	}
}

void MatrixHandler::outputMatriciesMap(const MatricesMap& matrices, const std::string& pattern) {
	for(const auto& mat : matrices)
		outputMatrix(mat.second, pattern + to_string(mat.first+1));
}


template<typename T>
void MatrixHandler::outputMatrix(const vecmath::TMatrix<T>& mat, const std::string& pattern) {
	std::ofstream stream;
	stream.exceptions(std::ofstream::badbit | std::ofstream::failbit);
	stream.open(pattern + ".dat");
	outputMatrix(stream, mat);
}

template<typename T>
void MatrixHandler::outputMatrix(std::ostream& str, const vecmath::TMatrix<T>& matrix) {
	str << "!Xmin=0.,Xmax=" << matrix.cols()
	    << ".,Ymin=0.,Ymax=" << matrix.rows() << ".\n";
	str << std::fixed;
	for(size_t iy = 0; iy < matrix.rows(); iy++) {
		for(size_t ix=0; ix < matrix.cols(); ix++) {
			if(matrix(iy, ix) != 0)
				str << std::setprecision(6) << matrix(iy, ix) << '\t';
			else
				str << '-' << '\t';
		}
		str << '\n';
	}
}
