#include "settings.h"

Settings* Settings::singleton = 0;


Settings::Settings() :
		threadCount_(4),
		updateInterval_(125),
		selectionRad(7),
		maxScaleFactor_(16),
		scaleSliderTicks_(1024),
		coordinatesPrecision_(9),
		zoom(0.9),
		altZoom(0.25),
		wheelZoom(0.8),
		moveDist(16),
		shiftMoveDist(1),
		altMoveDist(256),
		altShiftMoveDist(256),
		selPen1(Qt::white),
		selPen2(Qt::black),
		ptPen(Qt::white),
		selPtPen(Qt::white),
		selectionMode_(0)
{

	ptPen.setWidthF(1.5);

	selPen1.setStyle(Qt::DashLine);
	selPen2.setStyle(Qt::DashLine);

	selPen1.setWidthF(1.5);
	selPen2.setWidthF(1.5);

	QVector<qreal> dashPattern;
	dashPattern << 4 << 4;

	selPen1.setDashPattern(dashPattern);
	selPen2.setDashPattern(dashPattern);

	selPen1.setDashOffset(0);
	selPen2.setDashOffset(4);
}

Settings* Settings::settings() {
	if(!singleton) {
		singleton = new Settings();
	}

	return singleton;
}

int Settings::threadCount() const {
	return threadCount_;
}

int Settings::updateInterval() const {
	return updateInterval_;
}

int Settings::selectionRadius() const {
	return selectionRad;
}

qreal Settings::maxScaleFactor() const {
	return maxScaleFactor_;
}

int Settings::scaleSliderTicks() const {
	return scaleSliderTicks_;
}

int Settings::coordinatesPrecision() const {
	return coordinatesPrecision_;
}


qreal Settings::zoomFactor() const {
	return zoom;
}

qreal Settings::altZoomFactor() const {
	return altZoom;
}

qreal Settings::wheelZoomFactor() const {
	return wheelZoom;
}

int Settings::moveDistance() const {
	return moveDist;
}

int Settings::shiftMoveDistance() const {
	return shiftMoveDist;
}

int Settings::altMoveDistance() const {
	return altMoveDist;
}

int Settings::altShiftMoveDistance() const {
	return altShiftMoveDist;
}

const QPen& Settings::selectionPen1() const {
	return selPen1;
}

const QPen& Settings::selectionPen2() const {
	return selPen2;
}

const QPen& Settings::pointPen() const {
	return ptPen;
}

int Settings::selectionMode() {
	return selectionMode_;
}

void Settings::setSelectionMode(int selectionMode) {
	selectionMode_ = selectionMode;
}


const QPen& Settings::selectedPointPen() const {
	return selPtPen;
}

/*
Mandelbrot<double>* Settings::mandelbrotDefault() const {
	return new Mandelbrot<double>(640, 480, 2, Transformation<double>(4, 0, 0, 4, -2, -2));
}


IFS<double>* Settings::sierpinski() const {
	QList< Transformation<double> > affs;
	QList<QColor> colors;

	affs.push_back(Transformation<double>(0, 0, 0, 0.16, 0, 0));
	affs.push_back(Transformation<double>(0.2, -0.26, 0.23, 0.22, 0, 1.6));
	affs.push_back(Transformation<double>(-0.15, 0.28, 0.26, 0.24, 0, 0.44));
	affs.push_back(Transformation<double>(0.85, 0.04, -0.04, 0.85, 0, 1.6));

	affs.push_back(Transformation<double>(0.5, 0, 0, 0.5, 1, 0));
	affs.push_back(Transformation<double>(0.5, 0, 0, 0.5, 1, 1));
	affs.push_back(Transformation<double>(0.5, 0, 0, 0.5, 0, 1));

	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, -1, -1));
	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, 1, -1));
	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, -1, 1));
	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, 1, 1));
	affs.push_back(Transformation<double>(0.95, 0.2, -0.2, 0.95, 0, 0));

	colors.push_back(QColor(255, 255, 0));
	colors.push_back(QColor(255, 0, 0));
	colors.push_back(QColor(0, 0, 255));
	colors.push_back(QColor(0, 255, 0));

	return new IFS<double>(400, 400, Transformation<double>(4, 0, 0, 4, -2, -2), affs, colors);
}

IFS<double>* Settings::fern() const {
	QList< Transformation<double> > affs;
	QList<QColor> colors;

	affs.push_back(Transformation<double>(0, 0, 0, 0.16, 0, 0));
	affs.push_back(Transformation<double>(0.2, -0.26, 0.23, 0.22, 0, 1.6));
	affs.push_back(Transformation<double>(-0.15, 0.28, 0.26, 0.24, 0, 0.44));
	affs.push_back(Transformation<double>(0.85, 0.04, -0.04, 0.85, 0, 1.6));

	colors.push_back(QColor(255, 255, 0));
	colors.push_back(QColor(255, 0, 0));
	colors.push_back(QColor(0, 0, 255));
	colors.push_back(QColor(0, 255, 0));

	return new IFS<double>(300, 600, Transformation<double>(6, 0, 0, -6, -3, 3), affs, colors);
}

IFS<double>* Settings::ifsDefault() const {
	QList< Transformation<double> > affs;
	QList<QColor> colors;

	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, -1, -1));
	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, 1, -1));
	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, -1, 1));
	affs.push_back(Transformation<double>(0.2, 0, 0, 0.2, 1, 1));
	affs.push_back(Transformation<double>(0.95, 0.2, -0.2, 0.95, 0, 0));

	colors.push_back(QColor(255, 255, 0));
	colors.push_back(QColor(255, 0, 0));
	colors.push_back(QColor(0, 0, 255));
	colors.push_back(QColor(0, 255, 0));

	return new IFS<double>(400, 400, Transformation<double>(4, 0, 0, 4, -2, -2), affs, colors);
}


MagneticPendulum<double>* Settings::magneticPendulumDefault() const {
	QList<QPointF> magnets;
	QList<qreal> strengths;
	QList<QColor> colors;

	int nr = 3;
	qreal pi = 3.141529;

	for(int i = 0; i < nr; i++) {

		qreal arc = 2 * pi * i / nr;
		qreal rad = 2;

		magnets.push_back(QPointF(rad * cos(arc),
					  rad * sin(arc)));
		strengths.push_back(1);
		colors.push_back(QColor::fromHsv(360 * i / nr, 255, 255));

	}

	qreal friction = 0.07,
	height = 0.25,
	stepSize = 0.02,
	pullBackForce = 0.25,
	abortVelocity = 0.05,
	abortRadius = 0.1;
	int minIterations = 1000;
	int maxIterations = 80000;

	return new MagneticPendulum<double>(320, 320, 2,
					Transformation<double>(12, 0, 0, 12, -6, -6),
					magnets, strengths, colors,
					friction, height, stepSize,
					pullBackForce, abortVelocity, abortRadius,
					minIterations, maxIterations);
}

*/
