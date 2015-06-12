#ifndef TRACKEVENTHANDLER_HPP
#define TRACKEVENTHANDLER_HPP

#include <fstream>
#include "tdcdata/event.hpp"
#include "trek/trekhandler.hpp"

class TracksHandler : public tdcdata::AbstractEventHandler {
	using ChamberStreams = std::unordered_map<std::uintmax_t, std::ofstream*>;
  public:
	TracksHandler(const trek::ChamberConfig& config, const std::string& dirPath = ".");
	~TracksHandler();
	void needProjection(bool flag)  {mNeedProjections = flag;}
	void needTracks(bool flag)		{mNeedTracks = flag;}
	void handleEvent(const tdcdata::TUEvent& event) override;
	void flush() override;
  protected:
	void printChamberTracks(uintmax_t chamNum, const trek::Chamber& chamber, const tdcdata::UraganEvent& uEvent);
	void printTrack(const vecmath::Line3& tTrack, const vecmath::Line3& uTrack);
	void createProjectionStream(uintmax_t chamNum);
  private:
	trek::TrekHandler mTrekHandler;
	ChamberStreams mProjectionStreams;
	std::ofstream  mTracksStream;
	std::string    mDirPath;

	bool mNeedProjections;
	bool mNeedTracks;
};

#endif // TRACKEVENTHANDLER_HPP
