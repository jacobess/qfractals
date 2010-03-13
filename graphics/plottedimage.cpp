#include <math.h>
#include "plottedimage.h"

PlottedImage::PlottedImage(int width, int height) :
	img(width, height),
	maxDensity(0),
	r(width * height),
	g(width * height),
	b(width * height),
	density(width * height),
	refreshNeeded(false) {}

PlottedImage::~PlottedImage() {}

int PlottedImage::width() const {
	return img.width();
}

int PlottedImage::height() const {
	return img.height();
}

const QImage& PlottedImage::image() const {
	return img.image();
}

void PlottedImage::clear() {
	img.clear();
	std::fill(density.data(), density.data() + width() * height(), 0);
}

void PlottedImage::init() {
	maxDensity = 0;
}

void PlottedImage::setSize(int w, int h) {
	img.setSize(w, h);

	int size = w * h;

	r.resize(size);
	g.resize(size);
	b.resize(size);
	density.resize(size);

	std::fill(density.data(), density.data() + size, 0);
}

void PlottedImage::scale(int cx, int cy, qreal factor) {
	img.scale(cx, cy, factor);
	std::fill(density.data(), density.data() + width() * height(), 0);
}

void PlottedImage::move(int dx, int dy) {
	img.move(dx, dy);
	std::fill(density.data(), density.data() + width() * height(), 0);
}

void PlottedImage::select(qreal wx, qreal wy, qreal hx, qreal hy, qreal x0, qreal y0) {
	img.select(wx, wy, hx, hy, x0, y0);
	std::fill(density.data(), density.data() + width() * height(), 0);
}

void PlottedImage::refreshImage() {
	if(refreshNeeded) {
		refreshNeeded = false;

		for(int y = 0; y < height(); y++) {
			for(int x = 0; x < width(); x++) {
				updatePix(x, y);
			}
		}
	}
}

void PlottedImage::updatePix(int x, int y) {
	int index = x + width() * y;

	float d;

	if(maxDensity <= 1) {
		d = density[index];
	} else if(density[index] <= 1) {
		d = density[index] / maxDensity;
	} else {
		d = log(density[index]) / log(maxDensity);

		//d = log(d * 5 + 1) / log(6);
	}

	float r0 = r[index];
	float g0 = g[index];
	float b0 = b[index];

	img.setRgba(x, y, r0 * d, g0 * d, b0 * d, 1);
}


void PlottedImage::addDot(qreal x, qreal y, float r, float g, float b) {
	int x0 = (int) floor(x);
	int y0 = (int) floor(y);

	float degX = x - x0, degY = y - y0;

	float d00 = (1 - degX) * (1 - degY);
	float d10 = degX * (1 - degY);
	float d01 = (1 - degX) * degY;
	float d11 = degX * degY;

	addDot(x0, y0, r, g, b, d00);
	addDot(x0, y0 + 1, r, g, b, d01);
	addDot(x0 + 1, y0, r, g, b, d10);
	addDot(x0 + 1, y0 + 1, r, g, b, d11);
}

void PlottedImage::addDot(int x, int y, float r, float g, float b, float density) {
	if(x >= 0 && y >= 0 && x < width() && y < height() && density > 0) {
		int index = x + width() * y;

		float d0 = this->density[index];
		float r0 = this->r[index];
		float g0 = this->g[index];
		float b0 = this->b[index];

		float dt = d0 + density;

		this->r[index] = r0 * d0 / dt + r * density / dt;
		this->g[index] = g0 * d0 / dt + g * density / dt;
		this->b[index] = b0 * d0 / dt + b * density / dt;

		this->density[index] = dt;

		if(dt > maxDensity) {
			maxDensity = dt;
			refreshNeeded = true;
		}

		updatePix(x, y);
	}
}
