#include "colorpalette.h"

#include <cmath>
#include <map>

void ColorPalette::addColor(double x, double r, double g, double b) {
	r = r < 0 ? 0 : r > 1 ? 1 : r;
	g = g < 0 ? 0 : g > 1 ? 1 : g;
	b = b < 0 ? 0 : b > 1 ? 1 : b;

	channels_[0].add(x, r);
	channels_[1].add(x, g);
	channels_[2].add(x, b);
}

void ColorPalette::addChannelColor(int channel, double x, double c) {
	c = c < 0 ? 0 : c > 1 ? 1 : c;
	channels_[channel].add(x, c);
}

void ColorPalette::setPeriod(double p) {
	for(int i = 0; i < 3; i++) {
		channels_[i].setPeriod(p);
	}
}

void ColorPalette::setChannelPeriod(int channel, double p) {
	channels_[channel].setPeriod(p);
}

void ColorPalette::color(double x, double& r, double& g, double& b, double& a) const {
	a = 1 - opacity_(x);

	r = channels_[0](x);
	g = channels_[1](x);
	b = channels_[2](x);

	r = r < 0 ? 0 : r > 1 ? 1 : r;
	g = g < 0 ? 0 : g > 1 ? 1 : g;
	b = b < 0 ? 0 : b > 1 ? 1 : b;
}
