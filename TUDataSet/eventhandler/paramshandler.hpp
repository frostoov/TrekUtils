#ifndef CHARACTERISTICSHANDLER_HPP
#define CHARACTERISTICSHANDLER_HPP

#include "tdcdata/event.hpp"
#include "math/histogram.hpp"

namespace tdcdata {

class ParametersHandler : public AbstractEventHandler {
	using Histogram = vecmath::Histogram<uintmax_t>;
  public:
	ParametersHandler()
		: timeHist(0, 10000, 5), hasChars(false) {}
	virtual void handleEvent(const TUEvent& event) override;
	virtual void flush() override;
	uint32_t getPedestal() {
		if(hasChars)
			return pedestal;
		throw std::runtime_error("ParametersHandler: no pedestal");
	}
	double getSpeed() {
		if(hasChars)
			return speed;
		throw std::runtime_error("ParametersHandler: no speed");
	}

  private:
	Histogram timeHist;
	uint32_t pedestal;
	double speed;

	bool hasChars;
};

} //tudata

#endif // CHARACTERISTICSHANDLER_HPP
