#ifndef RENDEREDIMAGE_H
#define RENDEREDIMAGE_H

#include "imagewrapper.h"
#include <QMutex>

class ColorProvider {
public:
	virtual void color(uchar type, float val,
			   float& r, float& g, float& b, float& a) const = 0;
};

class RenderedImage : public Image {

	ImageWrapper img_;
	int aaDeg_;

	// type 255 = pixel is clear
	uchar* types_;
	float* vals_;

	const ColorProvider* colorProvider_;

public:
	RenderedImage(int width, int height, int aaDeg, const ColorProvider* colorProvider);
	virtual ~RenderedImage();

	RenderedImage& operator=(const RenderedImage& img);

	int width() const;
	int height() const;

	int pointsPerPix() const;

	const QImage& image();
	const QImage& image() const;

	void setColorProvider(const ColorProvider* colorProvider);

	void setSize(int width, int height);

	void setSize(int width, int height, int aaDeg);

	// Sets min/max-Values according to data array
	// 0 if no such type
	void init();

	void clear();
	void refreshImage();

	// Modify the image
	void scale(int cx, int cy, qreal factor);
	void move(int dx, int dy);
	void select(qreal wx, qreal wy, qreal hx, qreal hy, qreal x0, qreal y0);

	bool isClear(int x, int y, int index) const;

	qreal pointX(int x, int index) const;
	qreal pointY(int y, int index) const;

	void setPix(int x, int y, int index, uchar type, float val);

protected:
	void updatePix(int x, int y);
	int pix(int x, int y, int index) const;
};
#endif // RENDEREDIMAGE_H
