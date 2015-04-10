#include <iomanip>
#include <cmath>
#include <stdexcept>
#include <iostream>

#include "trek/chamber.hpp"
#include "event.hpp"
#include "eventhandler.hpp"

namespace tdcdata {

using std::ofstream;
using std::to_string;
using std::setw;
using std::setfill;
using std::atan;
using std::pair;
using std::string;

using vecmath::Vec3;

static constexpr double PI    = 3.14159265358979323846;
static constexpr double todeg = 180./PI;

EventHandler::EventHandler(const EventHandler::ChamberConfig& config, uint32_t pedestal, double speed)
	: mChamHandler(pedestal, speed), mMatrixN(matrixRows, matrixCols) {
	fullEmpty   = 0;
	emptyEvents = 0;
	badEvents   = 0;
	goodEvents  = 0;
	using trek::Chamber;
	for(const auto& chamberPos : config)
		mChamberSystems.insert( {chamberPos.first, Chamber::getChamberSystem(chamberPos.second.points)} );
}

void EventHandler::handleEvent(const TUEvent& event) {

	if(mHandleFlags.tracks)
		printProjection(event);
	if(mHandleFlags.listing)
		printListing(event);
	if(mHandleFlags.matrix)
		loadMatrix(event);
}

void EventHandler::flush() {
	if(mHandleFlags.matrix) {
		outputMatriciesMap();
		std::cout << "good      = " << goodEvents << '\n';
		std::cout << "bad       = " << badEvents << '\n';
		std::cout << "empty     = " << emptyEvents << '\n';
		std::cout << "fullEmpty = " << fullEmpty << '\n';
		goodEvents  = 0;
		badEvents   = 0;
		emptyEvents = 0;
	}
	if(mHandleFlags.tracks)
		for(auto& stream : mTrackStreams)
			stream.second->flush();
	if(mHandleFlags.listing)
		for(auto& stream : mListStreams)
			stream.second->flush();
}

void EventHandler::setFlags(const HandleFlags& flags) {
	mHandleFlags = flags;
}

void EventHandler::clear() {
	for(auto& stream : mTrackStreams)
		delete stream.second;
	for(auto& stream : mListStreams)
		delete stream.second;
	mTrackStreams.clear();
	mListStreams.clear();
	mMatricesT.clear();
	mMatrixN.fill();
}

void EventHandler::closeStreams() {
	for(auto& stream : mTrackStreams)
		stream.second->close();
	for(auto& stream : mListStreams)
		stream.second->close();
}

void EventHandler::outputMatriciesMap() {
	outputMatrix(mMatrixN, "matrixN");
	outputMatriciesMap(mMatricesT,"matrixT");
	DMatrix matrixE(161,161);
	for(const auto& matrixTPair : mMatricesT) {
		auto& matrixT = matrixTPair.second;
		auto cham = matrixTPair.first;
		for (size_t i = 0; i < matrixE.rows(); ++i)
			for (size_t j = 0; j < matrixE.columns(); ++j)
				if(mMatrixN(i,j) != 0)
					matrixE(i, j) =  static_cast<double>(matrixT(i, j)) / mMatrixN(i, j);
		outputMatrix(matrixE,"matrixE" + to_string(cham+1));
	}
}

void EventHandler::outputMatriciesMap(const MatricesMap& matrices, const string& pattern) {
	for(const auto& mat : matrices)
		outputMatrix(mat.second, pattern + to_string(mat.first+1));
}

void EventHandler::loadMatrix(const TUEvent& event) {
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

	TPoint3D P1 = {
		uraganEvent.chp0[0],
		uraganEvent.chp0[1],
		uraganEvent.chp0[2],
	};
	TPoint3D P2 = {
		uraganEvent.chp1[0],
		uraganEvent.chp1[1],
		uraganEvent.chp1[2],
	};

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

	if(ThetaG >= 0. && ThetaG < 20. && uraganEvent.trackID == 0 ) {
		//Найдём пересечение треком горизонтальной плоскости
		double z = 10880;
		double t = (z-P1.z)*iz/dz;
		double x = P1.x-3500+iz*dx*t; 	// 3500  - левый  край триггируемой области УРАГАНА
		double y = P1.y-12250+iz*dy*t; 	// 12250 - нижний край триггируемой области УРАГАНА
		size_t ix = (x/22.+0)+0.5;		// 22    - размер ячейки
		size_t iy = (y/22.+0)+0.5;
		if(ix < mMatrixN.columns() && iy < mMatrixN.rows()) {
			++mMatrixN(iy, ix);
			auto triggChambers = event.getTriggeredChambers();
			if(ix >= 113 && ix <= 133) {
				if(!triggChambers.count(16)) {
					emptyEvents++;
					if(triggChambers.empty())
						++fullEmpty;
				} else if(!eventChecker(event.getTrekEventRaw(), 16))
					badEvents++;
				else
					goodEvents++;
			}
			for(auto cham : triggChambers) {
				if(!mMatricesT.count(cham))
					mMatricesT.insert({cham, Matrix(matrixRows,matrixCols)});
				auto& tMatrix = mMatricesT.at(cham);
				if(eventChecker(event.getTrekEventRaw(), cham) &&
				        iy < tMatrix.rows() && ix < tMatrix.columns())
					++tMatrix(iy, ix);
			}
		}
	}
}

void EventHandler::printProjection(const TUEvent& event) {
	using trek::Chamber;
	auto trekEvent = event.getTrekEvent();
	for(const auto& chamEvent : trekEvent) {
		//chamEvent.first  - номер камеры
		//chamEvent.second - событие

		if( !mChamberSystems.count(chamEvent.first) )
			continue;

		auto& system = mChamberSystems.at(chamEvent.first);
		//Загружаем данные
		mChamHandler.setChamberData(chamEvent.second);
		if( !mChamHandler.hasChamberTrack() )
			continue;

		//Если для камеры нет потока, то создаем его
		if(!mTrackStreams.count(chamEvent.first))
			createTrackStream(mTrackStreams, chamEvent.first);
		auto& str = *mTrackStreams.at(chamEvent.first);

		auto& track  = mChamHandler.getChamberTrack();
		auto  uragan = Chamber::getUraganProjection(event.getUraganEvent().chp0,
		               event.getUraganEvent().chp1,
		               system);

		int32_t k1 = track.times[0] - track.times[1] - track.times[2] + track.times[3];
		int32_t k2 = track.times[0] - 3*track.times[1] + 3*track.times[2] - track.times[3];

		for(auto distance : track.times)
			str << setw(8) << setfill(' ') << distance << '\t';
		auto trackAngle = atan(track.line.k()) * todeg;
		str << setw(8)  << setfill(' ') << k1 << '\t'
		    << setw(8)  << setfill(' ') << k2 << '\t'
		    << setw(8)  << setfill(' ') << track.dev << '\t'
		    << setw(8)  << setfill(' ') << trackAngle << '\t'
		    << setw(8)  << setfill(' ') << track.line.b();
		auto uraganAngle = atan(uragan.k()) * todeg;
		str << '\t';
		str << setw(8)  << setfill(' ') << uraganAngle << '\t'
		    << setw(8)  << setfill(' ') << uragan.b() << '\t'
		    << setw(8)  << setfill(' ') << trackAngle - uraganAngle << '\t'
		    << setw(8)  << setfill(' ') << track.line.b() - uragan.b();
		str << '\n';
	}
}

void EventHandler::printListing(const TUEvent& event) {
	auto trekEvent = event.getTrekEvent();
	for(const auto& chamEvent : trekEvent) {
		//Если для камеры нет потока, то создаем его
		if(!mListStreams.count(chamEvent.first))
			createListStream(mListStreams, chamEvent.first);

		auto& str = *mListStreams.at(chamEvent.first);
		size_t depth = 0;
		for(const auto& wireData : chamEvent.second) {
			if(wireData.size() > depth)
				depth = wireData.size();
		}
		for(size_t i = 0; i < depth; ++i) {
			for(const auto& wireData : chamEvent.second) {
				if(i < wireData.size())
					str << setw(8) << setfill(' ') << wireData.at(i) << '\t';
				else
					str << setw(8) << setfill('-') << "-" << '\t';
			}
			str << '\n';
		}
	}
}

void EventHandler::createTrackStream(StreamsMap& streams, uintmax_t cham) {
	if(streams.count(cham))
		return;
	auto str = new ofstream;
	str->exceptions( ofstream::failbit | ofstream::badbit );
	str->open("tracks" + to_string(cham+1) + ".txt", ofstream::binary);

	*str << "Chamber №" << cham+1 << '\n';
	for(auto i = 0; i < 4 ; ++i)
		*str << setw(6) << setfill(' ') << "WIRE "
		     << setw(2) << setfill('0') << i
		     << setw(1) << setfill(' ') << '\t';
	*str << setw(8)  << setfill(' ') << "k1" << '\t'
	     << setw(8)  << setfill(' ') << "k2" << '\t'
	     << setw(8)  << setfill(' ') << "dev" << '\t'
	     << setw(8)  << setfill(' ') << "ang" << '\t'
	     << setw(8)  << setfill(' ') << "b";
	*str << '\t';
	*str << setw(8)  << setfill(' ') << "ang[u]" << '\t'
	     << setw(8)  << setfill(' ') << "b  [u]" << '\t'
	     << setw(8)  << setfill(' ') << "dang"   << '\t'
	     << setw(8)  << setfill(' ') << "db";
	*str << '\n';
	streams.insert({cham, str});
}

void EventHandler::createListStream(StreamsMap& streams, uintmax_t cham) {
	if(streams.count(cham))
		return;
	auto str = new ofstream;
	str->exceptions( ofstream::failbit | ofstream::badbit );
	str->open("listing" + to_string(cham+1) + ".txt", ofstream::binary);

	*str << "Chamber №" << cham+1 << '\n';
	for(auto i = 0; i < 4 ; ++i)
		*str << setw(6) << setfill(' ') << "WIRE "
		     << setw(2) << setfill('0') << i
		     << setw(1) << setfill(' ') << '\t';
	*str << '\n';

	streams.insert({cham, str});
}

template<typename T>
void EventHandler::outputMatrix(const vecmath::TMatrix<T>& mat, const std::string& pattern) {
	std::ofstream stream;
	stream.exceptions(std::ofstream::badbit | std::ofstream::failbit);
	stream.open(pattern + ".dat");
	outputMatrix(stream, mat);
}

template<typename T>
void EventHandler::outputMatrix(std::ostream& str, const vecmath::TMatrix<T>& matrix) {
	str << "!Xmin=0.,Xmax=" << matrix.columns()
	    << ".,Ymin=0.,Ymax=" << matrix.rows() << ".\n";
	str << std::fixed;
	for(size_t iy = 0; iy < matrix.rows(); iy++) {
		for(size_t ix=0; ix < matrix.columns(); ix++) {
			if(matrix(iy, ix) != 0)
				str << std::setprecision(6) << matrix(iy, ix) << '\t';
			else
				str << '-' << '\t';
		}
		str << '\n';
	}
}

} //tudata
