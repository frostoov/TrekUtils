#include <iomanip>
#include "trackshandler.hpp"

using trek::ChamberConfig;
using trek::Chamber;
using std::ofstream;
using std::setw;
using std::setfill;
using std::to_string;
using std::atan;
using tdcdata::TUEvent;
using tdcdata::UraganEvent;
using vecmath::toDeg;
using vecmath::Line3;

TracksHandler::TracksHandler(const ChamberConfig& config, const std::string& dirPath)
	: mTrekHandler(config),
	  mDirPath(dirPath),
	  mNeedProjections(false),
	  mNeedTracks(false) { }

TracksHandler::~TracksHandler() {
	for(auto& stream : mProjectionStreams)
		delete stream.second;
}

void TracksHandler::handleEvent(const ::TUEvent& event) {
	if(mNeedProjections || mNeedTracks) {
		mTrekHandler.loadEvent(event);
		if(mNeedProjections) {
			for(const auto& chamberPair : mTrekHandler.getChambers() ) {
				const auto  chamberNumber = chamberPair.first;
				const auto& chamber = chamberPair.second;
				if(chamber.hasTrack())
					printChamberTracks(chamberNumber, chamber, event.getUraganEvent());
			}
		}
		if(mNeedTracks && mTrekHandler.createTrack())
			printTrack(mTrekHandler.getTTrack(), mTrekHandler.getUTrack());
	}
}

void TracksHandler::flush() {
	for(auto& stream : mProjectionStreams) {
		if(stream.second)
			stream.second->flush();
	}
}

void TracksHandler::printChamberTracks(uintmax_t chamNum, const Chamber& chamber,
									   const UraganEvent& uEvent) {
	if(mProjectionStreams.count(chamNum) == 0)
		createProjectionStream(chamNum);

	auto& str = *mProjectionStreams.at(chamNum);

	auto& track  = chamber.getTrackDescription();
	auto  uragan = chamber.getUraganProjection(uEvent.chp0, uEvent.chp1);

	int32_t k1 = track.times[0] - track.times[1] - track.times[2] + track.times[3];
	int32_t k2 = track.times[0] - 3 * track.times[1] + 3 * track.times[2] - track.times[3];

	for(auto distance : track.times)
        str << distance << '\t';
	auto trackAngle = toDeg( atan(track.line.k()) );
    str << k1 << '\t'
        << k2 << '\t'
        << track.deviation << '\t'
        << trackAngle << '\t'
        << track.line.b();
	auto uraganAngle = toDeg( atan(uragan.k()));
	str << '\t';
    str << uraganAngle << '\t'
        << uragan.b() << '\t'
        << trackAngle - uraganAngle << '\t'
        << track.line.b() - uragan.b();
	str << '\n';

}

void TracksHandler::printTrack(const Line3& tTrack, const Line3& uTrack) {
	auto uVec = uTrack.vec().ort();
	auto tVec = tTrack.vec().ort();

	double angle = toDeg( uVec.angle(tVec) );
	if(angle > 90)
		angle = 180 - angle;

	if(!mTracksStream.is_open()) {
		mTracksStream.exceptions(ofstream::failbit | ofstream::badbit);
		mTracksStream.open(mDirPath + "/tracks.txt", ofstream::binary);
	}
	mTracksStream << angle << '\n';
}

void TracksHandler::createProjectionStream(uintmax_t chamNum) {
	if(!mProjectionStreams.count(chamNum)) {
		auto str = new ofstream;
		str->exceptions(ofstream::failbit | ofstream::badbit);
		str->open(mDirPath + "/projections" + to_string(chamNum + 1) + ".txt", ofstream::binary);

		*str << "Chamber №" << chamNum + 1 << '\n';
		for(auto i = 0; i < 4 ; ++i)
            *str << "WIRE" << setw(2) << setfill('0') << i << '\t';
        *str << "k1" << '\t'
             << "k2" << '\t'
             << "dev" << '\t'
             << "ang" << '\t'
             << "b";
		*str << '\t';
        *str << "ang[u]" << '\t'
             << "b[u]"   << '\t'
             << "dang"   << '\t'
             << "db";
		*str << '\n';
		mProjectionStreams.insert({chamNum, str});
	}
}
