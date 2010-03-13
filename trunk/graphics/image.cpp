#include "image.h"

qreal Image::normX(qreal x) const {
	int w = width();
	int h = height();

	int min = w > h ? h : w;

	return (x - (w - min) / 2.) / min;
}

qreal Image::normY(qreal y) const {
	int w = width();
	int h = height();

	int min = w > h ? h : w;

	return (y - (h - min) / 2.) / min;
}

qreal Image::denormX(qreal x0) const {
	int w = width();
	int h = height();

	int min = w > h ? h : w;

	return x0 * min + (w - min) / 2;
}

qreal Image::denormY(qreal y0) const {
	int w = width();
	int h = height();

	int min = w > h ? h : w;

	return y0 * min + (h - min) / 2;
}
