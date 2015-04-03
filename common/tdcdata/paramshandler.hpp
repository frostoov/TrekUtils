#ifndef CHARACTERISTICSHANDLER_HPP
#define CHARACTERISTICSHANDLER_HPP

#include "event.hpp"
#include "math/histogram.hpp"

namespace tdcdata {

class ParametersHandler : public AbstractEventHandler {
	using Histogram = vecmath::Histogram<uintmax_t>;
public:
	ParametersHandler()
		: timeHist(0, 1, 5), hasChars(false) {}
	virtual void handleEvent(const TUEvent& event) override;
	virtual void flush() override;
	uint32_t getPedestal() {
		if(!hasChars)
			throw std::runtime_error("Characteristics Handler: no pedestal");
		return pedestal;
	}
	double getSpeed() {
		if(!hasChars)
			throw std::runtime_error("Characteristics Handler: no speed");
		return speed;
	}

private:
	Histogram timeHist;
	uint32_t pedestal;
	double speed;

	bool hasChars;
};

} //tudata

#endif // CHARACTERISTICSHANDLER_HPP
