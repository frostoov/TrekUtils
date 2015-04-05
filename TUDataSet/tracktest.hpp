#ifndef TRACKTEST_HPP
#define TRACKTEST_HPP

#include "tdcdata/event.hpp"
#include "trek/trekhandler.hpp"

class TrackTest : public tdcdata::AbstractEventHandler
{
	using ChamberConfig = trek::ChamberConfig;
	using TrekHandler = trek::TrekHandler;
	using TUEvent = tdcdata::TUEvent;
public:
	TrackTest(const ChamberConfig& config, uint32_t pedestal, double speed);
	~TrackTest();
	void handleEvent(const TUEvent& event) override;
	void flush() override {}
private:
	TrekHandler mTrekHandler;
};

#endif // TRACKTEST_HPP
