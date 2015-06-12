#include "parametershandler.hpp"
#include "tdcdata/event.hpp"

using trek::ChamberParameters;
using trek::WireParameters;
using tdcdata::TUEvent;

void ParametersHandler::handleEvent(const TUEvent& event) {
	for(const auto word : event.getTrekEventRaw()) {
		const auto measurement   = TUEvent::getMsr(word);
		const auto chamberNumber = TUEvent::getChamber(word);
		const auto wireNumber    = TUEvent::getWire(word);
		if(mHistograms.count(chamberNumber) == 0) {
			mHistograms.insert({chamberNumber, ChamberHistograms()});
			for(auto& chamHistogram : mHistograms.at(chamberNumber))
				chamHistogram.setParameters(0, 10000, 10);
		}
		auto& wireHistogram = mHistograms.at(chamberNumber).at(wireNumber);
		if(measurement > wireHistogram.getUp())
			wireHistogram.expand(measurement);
		wireHistogram.addValue(measurement);
	}
}

void ParametersHandler::flush() {
	mChamParams.clear();
	for(const auto& chamHistogramPair : mHistograms) {
		auto chamNumber     = chamHistogramPair.first;
		auto& chamHistogram = chamHistogramPair.second;
		for(size_t wireNumber = 0; wireNumber < chamHistogram.size(); ++wireNumber) {
			const auto& wireHistogram = chamHistogram.at(wireNumber);
			auto maxTime = wireHistogram.maxValue();
			Histogram valueHist(0, maxTime, 10);
			for(size_t i = 0; i < wireHistogram.size(); ++i) {
				if(wireHistogram.value(i) >= maxTime / 2)
					valueHist.addValue(wireHistogram.value(i));
			}
			auto maxProb = valueHist.maxValueRange() / 2;

			uint32_t t1 = 0, t2 = 0;
			for(size_t i = 0; i < wireHistogram.size(); ++i)
				if(wireHistogram.value(i) >= maxProb) {
					t1 = wireHistogram.range(i);
					break;
				}
			for(ssize_t i = wireHistogram.size() - 1; i >= 0; --i)
				if(wireHistogram.value(i) >= maxProb) {
					t2 = wireHistogram.range(i);
					break;
				}

			uint32_t offset(t1);
			auto delta = (t2 - t1);
			if(delta == 0)
				mChamParams[chamNumber].at(wireNumber) = WireParameters(offset, 0);
			else {
				double speed(250. / delta);
				mChamParams[chamNumber].at(wireNumber) = WireParameters(offset, speed);
			}
		}
	}
	mHistograms.clear();
}
