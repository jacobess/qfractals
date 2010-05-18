#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <sys/types.h>
#include "math/spline.h"

class ColorPalette
{
	Spline<double> a_;
	Spline<double> r_;
	Spline<double> g_;
	Spline<double> b_;

public:
	void color(double x, double& r, double& g, double& b, double& a) const;

	Spline<double>& r();
	Spline<double>& g();
	Spline<double>& b();
	Spline<double>& a();
};

#endif // COLORPALETTE_H
