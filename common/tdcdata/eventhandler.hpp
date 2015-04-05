#ifndef TUEVENTHANDLER_HPP
#define TUEVENTHANDLER_HPP

#include <unordered_map>
#include <fstream>

#include "math/coordsystem.hpp"

#include "event.hpp"
#include "trek/chamberhandler.hpp"
#include "math/matrix.hpp"

namespace tdcdata {

class EventHandler : public AbstractEventHandler {
	using CoordSystem3        = vecmath::CoordSystem3;
	using ChamberSystems      = std::unordered_map<uintmax_t, CoordSystem3>;
	using ChamberConfig       = trek::ChamberConfig;
	using ChamberEventHandler = trek::ChamberEventHandler;
	using Matrix	       = vecmath::TMatrix<uintmax_t>;
	using DMatrix	       = vecmath::TMatrix<double>;
	using StreamsMap       = std::unordered_map<std::uintmax_t, std::ofstream*>;
	using MatricesMap      = std::unordered_map<std::uintmax_t, Matrix>;
	using CoordSystemsMap  = std::unordered_map<std::uintmax_t, CoordSystem3>;
  public:
	struct HandleFlags {
		bool tracks;
		bool matrix;
		bool listing;
		HandleFlags& operator=(const HandleFlags& flags) {
			if(this != &flags) {
				tracks  = flags.tracks;
				matrix  = flags.matrix;
				listing = flags.listing;
			}
			return *this;
		}
	};
	EventHandler(const ChamberConfig& config, uint32_t pedestal, double speed);
	~EventHandler() {closeStreams(); clear();}

	virtual void handleEvent(const TUEvent& event) override;
	virtual void flush() override;
	void setFlags(const HandleFlags& flags);
	void clear();
	void closeStreams();
  protected:
	void outputMatriciesMap();
	void outputMatriciesMap(const MatricesMap& matrices, const std::string& pattern);
	void printTrack(const TUEvent& event);
	void printListing(const TUEvent& event);
	void loadMatrix(const TUEvent& event);
	void createTrackStream(StreamsMap& streams, uintmax_t cham);
	void createListStream(StreamsMap& streams, uintmax_t cham);

	template<typename T>
	void outputMatrix(const vecmath::TMatrix<T>& mat, const std::string& pattern);
	template<typename T>
	void outputMatrix(std::ostream& str, const vecmath::TMatrix<T>& matrix);
  private:
	ChamberEventHandler mChamHandler;
	ChamberSystems		mChamberSystems;
	StreamsMap			mTrackStreams;
	StreamsMap			mListStreams;
	Matrix				mMatrixN;
	MatricesMap			mMatricesT;
	CoordSystemsMap		mChamSystems;
	HandleFlags			mHandleFlags;

	static const uintmax_t		matrixRows = 161;
	static const uintmax_t		matrixCols = 161;
};

} //tudata

#endif // TUEVENTHANDLER_HPP
