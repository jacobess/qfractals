#include "colorpalette.h"

#include <cmath>
#include <map>

void ColorPalette::color(float x, float& r, float& g, float& b, float& a) const {
	r = r_(x);
	g = g_(x);
	b = b_(x);
	a = a_(x);
}

Spline<float>& ColorPalette::r() {
	return r_;
}

Spline<float>& ColorPalette::g() {
	return g_;
}

Spline<float>& ColorPalette::b() {
	return b_;
}

Spline<float>& ColorPalette::a() {
	return a_;
}

