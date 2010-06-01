#include "image.h"

double Image::normX(double x) const {
	int w = width();
	int h = height();

	int min = w > h ? h : w;

	return (x - (w - min) / 2.) / min;
}

double Image::normY(double y) const {
	int w = width();
	int h = height();

	int min = w > h ? h : w;

	return (y - (h - min) / 2.) / min;
}

double Image::denormX(double x0) const {
	int w = width();
	int h = height();

	int min = w > h ? h : w;

	return x0 * min + (w - min) / 2;
}

double Image::denormY(double y0) const {
	int w = width();
	int h = height();

	int min = w > h ? h : w;

	return y0 * min + (h - min) / 2;
}
