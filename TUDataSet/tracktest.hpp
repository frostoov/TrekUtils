#ifndef TRACKTEST_HPP
#define TRACKTEST_HPP

#include <fstream>
#include "tdcdata/event.hpp"
#include "trek/trekhandler.hpp"

class TrackTest : public tdcdata::AbstractEventHandler {
	using ChamberConfig = trek::ChamberConfig;
	using TrekHandler = trek::TrekHandler;
	using TUEvent = tdcdata::TUEvent;
  public:
	TrackTest(const ChamberConfig& config, uint32_t pedestal, double speed);
	void handleEvent(const TUEvent& event) override;
	void flush() override {stream.close();}
  private:
	TrekHandler mTrekHandler;
	std::ofstream stream;
};

#endif // TRACKTEST_HPP
