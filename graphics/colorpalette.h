#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <sys/types.h>
#include "math/spline.h"

class ColorPalette
{
	Spline opacity_; // alpha
	Spline channels_[3];
public:
	// Modify palette
	void addColor(double x, double r, double g, double b);

	void addChannelColor(int channel, double x, double c);

	void setPeriod(double p);
	void setChannelPeriod(int channel, double p);

	void color(double x, double& r, double& g, double& b, double& a) const;
};

#endif // COLORPALETTE_H
