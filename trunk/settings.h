#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

#include <QPen>

#include "ui/selectablewidget.h"

class Settings {

	// Generator Settings:
	int threadCount_;
	int updateInterval_;

	// GUI-Settings:

	// SelectableWidget:
	int selectionRad;
	qreal maxScaleFactor_;
	int scaleSliderTicks_;

	int coordinatesPrecision_;

	qreal zoom;
	qreal altZoom;
	qreal wheelZoom;

	int moveDist;
	int shiftMoveDist;
	int altMoveDist;
	int altShiftMoveDist;

	QPen selPen1;
	QPen selPen2;
	QPen ptPen;
	QPen selPtPen;

	int selectionMode_;

	static Settings* singleton;

	Settings();

public:

	static Settings* settings();

	/*Mandelbrot<double> *mandelbrotDefault() const;

	IFS<double> *sierpinski() const;
	IFS<double> *fern() const;
	IFS<double> *ifsDefault() const;

	Magnetic<double> *magneticDefault() const;*/

	int threadCount() const;
	int updateInterval() const;
	int refreshInterval() const;

	int selectionRadius() const;
	qreal maxScaleFactor() const;
	int scaleSliderTicks() const;
	int coordinatesPrecision() const;

	qreal zoomFactor() const;
	qreal altZoomFactor() const;
	qreal wheelZoomFactor() const;
	int moveDistance() const;
	int shiftMoveDistance() const;
	int altMoveDistance() const;
	int altShiftMoveDistance() const;

	const QPen& selectionPen1() const;
	const QPen& selectionPen2() const;
	const QPen& pointPen() const;
	const QPen& selectedPointPen() const;

	int selectionMode();
	void setSelectionMode(int mode);
};

#endif // SETTINGS_H
