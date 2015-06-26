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
using vecmath::todeg;
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
        str << setw(8) << setfill(' ') << distance << '\t';
    auto trackAngle = atan(track.line.k()) * todeg;
    str << setw(8)  << setfill(' ') << k1 << '\t'
        << setw(8)  << setfill(' ') << k2 << '\t'
        << setw(8)  << setfill(' ') << track.deviation << '\t'
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

void TracksHandler::printTrack(const Line3& tTrack, const Line3& uTrack) {
    auto uVec = uTrack.vec().ort();
    auto tVec = tTrack.vec().ort();

    double angle = uVec.angle(tVec) * todeg;
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
             << setw(8)  << setfill(' ') << "b[u]"   << '\t'
             << setw(8)  << setfill(' ') << "dang"   << '\t'
             << setw(8)  << setfill(' ') << "db";
        *str << '\n';
        mProjectionStreams.insert({chamNum, str});
    }
}
