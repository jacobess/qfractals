#include "colorpalette.h"

#include <cmath>
#include <map>

void ColorPalette::color(double x, double& r, double& g, double& b, double& a) const {
	r = r_(x);
	g = g_(x);
	b = b_(x);
	a = a_(x);
}

Spline<double>& ColorPalette::r() {
	return r_;
}

Spline<double>& ColorPalette::g() {
	return g_;
}

Spline<double>& ColorPalette::b() {
	return b_;
}

Spline<double>& ColorPalette::a() {
	return a_;
}

