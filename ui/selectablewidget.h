#ifndef SELECTABLEWIDGET_H
#define SELECTABLEWIDGET_H

#include <QtGui>

#include "fractals/viewport.h"

class SelectableWidget: public QWidget {
	Q_OBJECT

	ViewportProxy* vp_;

	// -1 = not selected, 0/1 yes, value is the select mode
	int hasSelection_;

	// For Browse Mode
	QPointF p0_, p1_;

	// For DragMode
	QPolygonF selectionPolygon_;

	bool moveSelection_;

	int selectedPointIndex_;

public:
	SelectableWidget(QWidget *parent, ViewportProxy* vp);

	void wheelEvent(QWheelEvent *event);

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	void mouseDoubleClickEvent(QMouseEvent *event);

	void keyPressEvent(QKeyEvent *event);

	void paintEvent(QPaintEvent *event);

signals:
	void status(QString message);

private:
	int findSelectablePoint(QPoint pos) const;
	void moveSelectedPoint(bool keepRatio, bool keepCentered, bool shear);

	// Translate Coordinates
	QPointF toImg(QPointF p) const;
	QPointF fromImg(QPointF p) const;
};

#endif // SELECTABLEWIDGET_H
