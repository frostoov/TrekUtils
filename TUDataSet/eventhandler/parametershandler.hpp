#ifndef PARAMETERS_HANDLER_HPP
#define PARAMETERS_HANDLER_HPP

#include "tdcdata/event.hpp"
#include "math/histogram.hpp"
#include "trek/trektypes.hpp"

class ParametersHandler : public tdcdata::AbstractEventHandler {
	using Histogram         = vecmath::THistogram<uintmax_t>;
	using ChamberHistograms = std::array<Histogram, 4>;
	using HistogramMap      = std::unordered_map<uintmax_t, ChamberHistograms>;
	using ParametersMap     = std::unordered_map<uintmax_t, trek::ChamberParameters>;
  public:
	virtual void handleEvent(const tdcdata::TUEvent& event) override;
	virtual void flush() override;
	const ParametersMap& getParameters() const {return mChamParams;}
  private:
	HistogramMap  mHistograms;
	ParametersMap mChamParams;
};

#endif // CHARACTERISTICSHANDLER_HPP
