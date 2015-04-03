#include <stdexcept>
#include "chamberrender.hpp"

QPixmap const& ChamberRender::getPixmap(int width, int height) {
	if(mHandler == nullptr)
		throw std::runtime_error("No mHandler for chamber render");

	if(mPixmapWidth != width && mPixmapHeight != height)
		mPixmap = QPixmap(width, height);

	double scaleX = (double)width/500.;
	double scaleY = (double)height/112.;

	mPixmap.fill();

	drawPixmap(mPixmap,scaleX,scaleY);
	mPixmapWidth	 = width;
	mPixmapHeight	 = height;
	return mPixmap;
}

void ChamberRender::drawPixmap(QPixmap& pix, double scaleX, double scaleY) {
	QPainter painter(&pix);
	painter.setRenderHint(QPainter::Antialiasing, true);
	drawWires(painter,scaleX,scaleY);
	drawChamberPoints(painter,scaleX,scaleY);
	drawChamberTracks(painter,scaleX,scaleY);
	drawUraganTracks(painter,scaleX,scaleY);
}

void ChamberRender::drawWires(QPainter& painter, double scaleX, double scaleY) {
	painter.setBrush(Qt::white);
	painter.setPen(Qt::black);

	int Y[] = {
		static_cast<int>(41 * scaleY),
		static_cast<int>(51 * scaleY),
		static_cast<int>(61 * scaleY),
		static_cast<int>(71 * scaleY),
	};
	int X[] = {
		static_cast<int>(245 * scaleX),
		static_cast<int>(255 * scaleX),
		static_cast<int>(245 * scaleX),
		static_cast<int>(255 * scaleX),
	};

	for(unsigned i = 0; i < 4; ++i)
		painter.drawEllipse(X[i] - 5, Y[i] - 5,
		                    10, 10);
}

void ChamberRender::drawChamberPoints(QPainter& painter, double scaleX, double scaleY) {
	if(!mHandler->hasChamberTrack())
		return;
	painter.setBrush(Qt::red);
	painter.setPen( {Qt::white, 0} );
	auto points = mHandler->getChamberTrack().points;
	convertPoints(points);
	int x,y;
	for(const auto p : points) {
		x = p.x() * scaleX;
		y = p.y() * scaleY;
		painter.drawEllipse(x - 5, y - 5, 10, 10);
	}
}

void ChamberRender::drawChamberTracks(QPainter& painter, double scaleX, double scaleY) {
	if(!mHandler->hasChamberTrack())
		return;
	painter.setPen( {Qt::green, 2} );
	auto trackPoints = createTrack( mHandler->getChamberTrack().line );
	convertPoints(trackPoints);
	auto lines = getLine(trackPoints,scaleX,scaleY);
	painter.drawLines(lines);
}

void ChamberRender::drawUraganTracks(QPainter& painter, double scaleX, double scaleY) {
	if(!mHandler->hasUraganTrack())
		return;
	painter.setPen({Qt::blue,2});
	auto uraganPoints = createTrack( mHandler->getUraganTrack() );
	convertPoints(uraganPoints);
	auto lines = getLine(uraganPoints,scaleX,scaleY);
	painter.drawLines(lines);
}

ChamberRender::pVector ChamberRender::createTracks(const lVector& lines) {
	pVector trackPoints;
	for(const auto line : lines )
		line.checkRectangle(0, 112, -250, 250, trackPoints);
	return trackPoints;
}

ChamberRender::pVector ChamberRender::createTrack(const Line2& line) {
	pVector uraganPoints;
	line.checkRectangle(0, 112, -250, 250, uraganPoints);
	return uraganPoints;
}

QVector<QLine> ChamberRender::getLine(const pVector& points, double scaleX, double scaleY) {
	QVector<QLine> lines;
	for(size_t i = 0,l = 1; l < points.size(); i += 2, l += 2)
		lines.push_back(QLine(points[i].x() * scaleX, points[i].y() * scaleY,
		                      points[l].x() * scaleX, points[l].y() * scaleY) );
	return lines;
}

void ChamberRender::convertPoints(pVector& points) {
	float x,y;
	for(auto& p : points) {
		x = p.y();
		y = -p.x();

		p.x() = x + 250;
		p.y() = y + 112;
	}
}

ChamberRender::Vec2 ChamberRender::convert(const Vec2& vec) {
	float x = vec.y();
	float y = vec.x();
	return Vec2(250 - x, 112 - y);
}
