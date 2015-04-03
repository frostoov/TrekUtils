#ifndef CHAMBERRENDER_HPP
#define CHAMBERRENDER_HPP

#include <QPixmap>
#include <QPainter>

#include "chamberhandler.hpp"

class ChamberRender {
	using ChamberEventHandler = trek::ChamberEventHandler;
	using Vec2		= vecmath::TVec2<double>;
	using Line2		= vecmath::TLine2<double>;
	using dVector	= std::vector<double>;
	using pVector	= std::vector<Vec2>;
	using lVector	= std::vector<Line2>;
  public:
	ChamberRender(ChamberEventHandler* man = nullptr)
		:mHandler(man), mPixmapWidth(0), mPixmapHeight(0) {}

	void setManager(ChamberEventHandler* handler) { mHandler = handler; }
	const QPixmap& getPixmap(int width, int height);
  protected:
	void drawPixmap			(QPixmap& pix, double scaleX, double scaleY);
	void drawWires			(QPainter& painter, double scaleX, double scaleY);
	void drawChamberPoints	(QPainter& painter, double scaleX, double scaleY);
	void drawChamberTracks	(QPainter& painter, double scaleX, double scaleY);
	void drawUraganTracks	(QPainter& painter, double scaleX, double scaleY);

	pVector createTracks	(const lVector& lines);
	pVector createTrack		(const Line2& line);
	void	convertPoints	(pVector& points);
	Vec2	convert			(const Vec2& vec);
	QVector<QLine> getLine(const pVector& points, double scaleX, double scaleY);
  private:
	ChamberEventHandler*	mHandler;
	QPixmap				mPixmap;
	int					mPixmapWidth;
	int					mPixmapHeight;
};

#endif // CHAMBERRENDER_HPP
