#include "paramshandler.hpp"
#include "event.hpp"

namespace tdcdata {

void ParametersHandler::handleEvent(const TUEvent& event) {
	for(auto word : event.getTrekEventRaw()) {
		auto msr = TUEvent::getMsr(word);
		if(msr > timeHist.getUp())
			timeHist.expand(msr);
		timeHist.addValue(msr);
	}
}

void ParametersHandler::flush() {
	auto maxTime = timeHist.maxValue();
	Histogram valueHist(0, maxTime, 10);
	for(size_t i = 0; i < timeHist.size(); ++i) {
		if(timeHist.value(i) >= maxTime/5)
			valueHist.addValue(timeHist.value(i));
	}
	auto maxProb = valueHist.maxValueRange();
	auto middleTimeHisto = timeHist.size()/2;

	uint32_t t1 = 0,t2 = 0;
	for(size_t i = middleTimeHisto; i < timeHist.size(); ++i)
		if(timeHist.value(i) <= maxProb/2) {
			t2 = timeHist.range(i);
			break;
		}
	for(ssize_t i = middleTimeHisto; i >= 0; --i)
		if(timeHist.value(i) <= maxProb/2) {
			t1 = timeHist.range(i);
			break;
		}
	pedestal = t1;
	auto delta = (t2 - t1);
	if(delta == 0)
		throw std::overflow_error("ParametersHandler::flush: Division by zero");
	speed = 250./delta;
	hasChars = true;
}

} //tudata
