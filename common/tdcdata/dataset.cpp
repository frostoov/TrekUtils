#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <list>
#include <cstring>
#include <unordered_set>

#include "dataset.hpp"

namespace tdcdata {

using sstream = std::stringstream;
using std::istream;
using std::ios_base;
using std::set;
using std::string;
using std::runtime_error;
using std::exception;
using std::ifstream;
using std::stringstream;
using std::list;
using std::unordered_set;


static size_t getStreamSize(istream& stream) {
    auto startPosition = stream.tellg();
    stream.seekg(0, ios_base::end);
    auto fileSize = static_cast<size_t>(stream.tellg());
    stream.seekg(startPosition);
    return fileSize;
}

bool DataSet::checkExtension(const string& fileName) {
    auto pos = fileName.find_last_of('.');
    if(pos != string::npos && fileName.substr(pos) == ".tds")
        return true;
    else
        return false;
}

void DataSet::read(const string& path) {
    clear();
    append(path);
}

std::set<uintmax_t> DataSet::getTriggeredChambers() {
    set<uintmax_t> triggChams;
    loadTriggeredChambers(triggChams);
    return triggChams;
}

void DataSet::loadTriggeredChambers(std::set<uintmax_t>& triggChams) {
    for(const auto& event : *this)
        event.loadTriggeredChambers(triggChams);
}

void DataSet::append(const string& path) {
    ifstream fileStream;
    fileStream.exceptions(ifstream::failbit | ifstream::badbit);
    fileStream.open(path, ifstream::binary);
    deserializeStream(fileStream);
}

void DataSet::deserializeStream(istream& stream) {
    auto streamSize = getStreamSize(stream);
    if(streamSize < mCurrentHeader.getSize() )
        throw runtime_error("deserializeStream: invalid file size");

    deserialize(stream, mCurrentHeader);
    if (!mCurrentHeader.isValid())
        throw runtime_error("deserializeStream: invalid file keyword");
    else if (mCurrentHeader.fileSize !=  streamSize )
        throw runtime_error("deserializeStream: invalid file size");
    parseStream(stream, streamSize);
}

void DataSet::parseStream(istream& stream, size_t streamSize) {
    uint32_t measurementSize;
    TUEvent event;
    auto koef = mCurrentHeader.getLSBKoef();

    while(stream) {
        if(static_cast<size_t>(stream.tellg()) + getSize<decltype(event.mUraganEvent)>() >= streamSize)
            break;
        deserialize(stream, event.mUraganEvent);
        deserialize(stream, measurementSize);
        if(streamSize < measurementSize)
            throw runtime_error("deserializeStream: invalid value of measurement size");
        event.mTrekEvent.resize(measurementSize / sizeof(uint32_t));
        deserialize(stream, event.mTrekEvent);
        convert(event.mTrekEvent, koef);
        push_back(event);
    }
}

void DataSet::convert(UIntVector& vec, double koef) const {
    uint32_t temp;
    for(auto& word : vec) {
        temp = TUEvent::getMsr( word ) * koef;
        temp &= 0x7FFFF;
        word &= 0xFFF80000;
        word |= temp;
    }
}

} //tudata
