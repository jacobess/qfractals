#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

#include <QPen>

#include "ui/selectablewidget.h"

class Settings {

	// Generator Settings:
	int threadCount_;
	int updateInterval_;
	int refreshInterval_;

	// GUI-Settings:

	// SelectableWidget:
	int selectionRad_;
	qreal maxScaleFactor_;
	int scaleSliderTicks_;

	int coordinatesPrecision_;

	qreal zoom_;
	qreal altZoom_;
	qreal wheelZoom_;

	int moveDist_;
	int shiftMoveDist_;
	int altMoveDist_;
	int altShiftMoveDist_;

	QPen selPen1_;
	QPen selPen2_;
	QPen ptPen_;
	QPen selPtPen_;

	int selectionMode_;

	// Settings
	QMap<QString, Specification*> specifications_;

	int defaultWidth_;
	int defaultHeight_;

	static Settings* singleton_;

private:
	Settings();
	void initSpecs();

public:
	static Settings* settings();

	const QMap<QString, Specification*>& specifications() const;

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

	int selectionMode() const;
	void setSelectionMode(int mode);

	int defaultWidth() const;
	int defaultHeight() const;
};

#endif // SETTINGS_H
