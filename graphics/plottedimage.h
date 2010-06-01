#ifndef PLOTTEDIMAGE_H
#define PLOTTEDIMAGE_H

#include "imagewrapper.h"
#include <QVarLengthArray>

class PlottedImage : public Image {

	ImageWrapper img;

	double maxDensity;

	QVarLengthArray<double> r;
	QVarLengthArray<double> g;
	QVarLengthArray<double> b;
	QVarLengthArray<double> density;
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

	void scale(int cx, int cy, double factor);
	void move(int dx, int dy);
	void select(double wx, double wy, double hx, double hy, double x0, double y0);

	void addDot(double x, double y, double r, double g, double b);

	void refreshImage();

protected:
	void addDot(int x, int y, double r, double g, double b, double density = 1);
	void updatePix(int x, int y);
};

#endif // PLOTTEDIMAGE_H
