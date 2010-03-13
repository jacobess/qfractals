#ifndef PLOTTEDIMAGE_H
#define PLOTTEDIMAGE_H

#include "imagewrapper.h"
#include <QVarLengthArray>

class PlottedImage : public Image {

	ImageWrapper img;

	float maxDensity;

	QVarLengthArray<float> r;
	QVarLengthArray<float> g;
	QVarLengthArray<float> b;
	QVarLengthArray<float> density;

	bool refreshNeeded;
public:
	PlottedImage(int width, int height);
	virtual ~PlottedImage();

	PlottedImage& operator=(const PlottedImage& img);

	int width() const;
	int height() const;

	const QImage& image();
	const QImage& image() const;

	// init resets the density-array and maxDensity
	void init();
	void clear();

	void setSize(int width, int height);

	void scale(int cx, int cy, qreal factor);
	void move(int dx, int dy);
	void select(qreal wx, qreal wy, qreal hx, qreal hy, qreal x0, qreal y0);

	void addDot(qreal x, qreal y, float r, float g, float b);

	void refreshImage();

protected:
	void addDot(int x, int y, float r, float g, float b, float density = 1);
	void updatePix(int x, int y);
};

#endif // PLOTTEDIMAGE_H
