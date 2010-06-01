#ifndef IMAGEWRAPPER_H
#define IMAGEWRAPPER_H

#include "image.h"

class ImageWrapper : public Image {
	QImage img_;
	uint* data_;

public:
	ImageWrapper(int width, int height);
	virtual ~ImageWrapper();

	int width() const;
	int height() const;

	const QImage& image() const;
	QImage& image();

	void refreshImage();

	void clear();

	void setSize(int width, int height);

	void scale(int cx, int cy, double factor);
	void move(int dx, int dy);
	void select(double wx, double wy, double hx, double hy, double x0, double y0);

	void rgba(int x, int y, float& r, float& g, float& b, float& a) const;
	void setRgba(int x, int y, float r, float g, float b, float a);
};

#endif // IMAGEWRAPPER_H
