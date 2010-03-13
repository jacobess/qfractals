#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <sys/types.h>
#include "math/spline.h"

class ColorPalette
{
	Spline<float> a_;
	Spline<float> r_;
	Spline<float> g_;
	Spline<float> b_;
public:
	void color(float x, float& r, float& g, float& b, float& a) const;

	Spline<float>& r();
	Spline<float>& g();
	Spline<float>& b();
	Spline<float>& a();
};

#endif // COLORPALETTE_H
