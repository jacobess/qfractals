#ifndef RENDEREDIMAGE_H
#define RENDEREDIMAGE_H

#include "imagewrapper.h"
#include <QMutex>

class ColorProvider {
public:
	virtual void color(uchar type, double value,
			   double& r, double& g, double& b, double& a) const = 0;
};

class RenderedImage : public Image {

	// Parameters
	int width_;
	int height_;
	int aaDeg_;

	// Data
	ImageWrapper img_;

	// type 255 = pixel is clear
	uchar* types_;
	double* vals_; // size is combination of img_ width and img_ height

	const ColorProvider* colorProvider_;

public:
	RenderedImage(int width, int height, int aaDeg, const ColorProvider* colorProvider = 0);
	virtual ~RenderedImage();

	int width() const;
	int height() const;
	int indicesPerPixel() const;

	const QImage& image() const;

	void setSize(int width, int height);

	void setSize(int width, int height, int aaDeg);

	void clear();

	void refreshImage();

	// Modify the image
	void scale(int cx, int cy, double factor);
	void move(int dx, int dy);
	void select(double wx, double wy, double hx, double hy, double x0, double y0);

	bool isClear(int x, int y, int index) const;

	double pointX(int x, int index) const;
	double pointY(int y, int index) const;

	bool pix(int x, int y, int index, double& r, double& g, double& b, double& a);
	bool pix(int x, int y, int index, uchar& type, double& value);

	void setPix(int x, int y, int index, uchar type, double value);

protected:
	void clearData();

	void updatePix(int x, int y);
};
#endif // RENDEREDIMAGE_H
