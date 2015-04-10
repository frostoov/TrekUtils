#ifndef CHAMBERRENDER_HPP
#define CHAMBERRENDER_HPP

#include <QPixmap>
#include <QPainter>

#include "chamberhandler.hpp"
#include "tdcdata/event.hpp"

class ChamberRender {
	using ChamberEventHandler = trek::ChamberHandler;
	using TrackDesc = ChamberEventHandler::TrackDesc;
	using ChamberEvent = tdcdata::TDCEvent::ChamberEvent;
	using Vec2		= vecmath::TVec2<double>;
	using Line2		= vecmath::TLine2<double>;
	using dVector	= std::vector<double>;
	using pVector	= std::vector<Vec2>;
	using lVector	= std::vector<Line2>;
  public:
	QPixmap getPixmap(int width, int height, const TrackDesc* tTrack,
	                  const Line2* uTrack, const ChamberEvent* event);
  protected:
	void drawPixmap(QPixmap& pix, double scaleX, double scaleY, const TrackDesc* tTrack,
	                const Line2* uTrack, const ChamberEvent* event);
	void drawWires			(QPainter& painter, double scaleX, double scaleY, const ChamberEvent* event);
	void drawChamberPoints  (QPainter& painter, double scaleX, double scaleY, const TrackDesc* tTrack);
	void drawChamberTracks  (QPainter& painter, double scaleX, double scaleY, const TrackDesc* tTrack);
	void drawUraganTracks	(QPainter& painter, double scaleX, double scaleY, const Line2* uTrack);

	pVector createTracks	(const lVector& lines);
	pVector createTrack		(const Line2& line);
	void	convertPoints	(pVector& points);
	Vec2	convert			(const Vec2& vec);
	QVector<QLine> getLine(const pVector& points, double scaleX, double scaleY);

};

#endif // CHAMBERRENDER_HPP
