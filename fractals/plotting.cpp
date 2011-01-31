#include <math.h>

#include "plotting.h"

template<class T>
Plotting<T>::Plotting(const Transformation<T>& t) :
	Viewport<T>(t) {}


template<class T>
Plotting<T>::~Plotting() {}

template<class T>
PlottingGenerator<T>::PlottingGenerator(int width, int height) :
		ViewportGenerator<T>(-1, width, height),
		maxDensity_(0),
		r_(width * height),
		g_(width * height),
		b_(width * height),
		density_(width * height)
{
		std::fill(density_.data(), density_.data() + width * height, 0);
}

template<class T>
PlottingGenerator<T>::~PlottingGenerator() {}

template<class  T>
int PlottingGenerator<T>::progress() const {
	return 0;
}

template<class  T>
int PlottingGenerator<T>::totalSteps() const {
	return 0;
}

template<class T>
double PlottingGenerator<T>::addDot(const T &x, const T &y, double r, double g, double b) {
	T tx = specification().transformation().fromX(x, y);
	T ty = specification().transformation().fromY(x, y);

	double x0 = this->denormX(tx);
	double y0 = this->denormY(ty);

	double dx = x0 < 0 ? -x0 : this->width() - 1 < x0 ? x0 - this->width() + 1 : 1;

	double dy = y0 < 0 ? -y0 : this->height() - 1 < y0 ? y0 - this->height() + 1 : 1;

	if(dx <= 0 || dy <= 0) {
		return 0;
	} else {
		addPix(x0, y0, r, g, b);
		return dx * dy;
	}
}

template<class T>
void PlottingGenerator<T>::clear() {
	ViewportGenerator<T>::clear();
	std::fill(density_.data(), density_.data() + this->width() * this->height(), 0);
}

template<class T>
void PlottingGenerator<T>::init() {
	maxDensity_ = 0;
}

template<class T>
void PlottingGenerator<T>::setSizeUnsafe(int w, int h) {
	ViewportGenerator<T>::setSizeUnsafe(w, h);

	int size = w * h;

	r_.resize(size);
	g_.resize(size);
	b_.resize(size);
	density_.resize(size);

	std::fill(density_.data(), density_.data() + size, 0);
}

template<class T>
void PlottingGenerator<T>::scaleUnsafe(int cx, int cy, double factor) {
	ViewportGenerator<T>::scaleUnsafe(cx, cy, factor);
	std::fill(density_.data(), density_.data() + this->width() * this->height(), 0);
}

template<class T>
void PlottingGenerator<T>::moveUnsafe(int dx, int dy) {
	ViewportGenerator<T>::moveUnsafe(dx, dy);
	std::fill(density_.data(), density_.data() + this->width() * this->height(), 0);
}

template<class T>
void PlottingGenerator<T>::selectUnsafe(double wx, double wy, double hx, double hy, double x0, double y0) {
	ViewportGenerator<T>::selectUnsafe(wx, wy, hx, hy, x0, y0);
	std::fill(density_.data(), density_.data() + this->width() * this->height(), 0);
}

template<class T>
void PlottingGenerator<T>::refreshUnsafe() {
	for(int y = 0; y < this->height() && !this->isInterrupted(); y++) {
		for(int x = 0; x < this->width() && !this->isInterrupted(); x++) {
			updatePix(x, y);
		}
	}
}

template<class T>
void PlottingGenerator<T>::updatePix(int x, int y) {
	int index = x + this->width() * y;

	double d;

	// TODO Add parameter to determine whether d should be
	// * always 1
	// * linear
	// * logarithmic
	// * something else?
	if(maxDensity_ <= 1) {
		d = density_[index];
	} else if(density_[index] <= 1) {
		d = density_[index] / maxDensity_;
	} else {
		d = log(density_[index]) / log(maxDensity_);
	}

	double r0 = r_[index];
	double g0 = g_[index];
	double b0 = b_[index];

	this->setRgba(x, y, r0 * d, g0 * d, b0 * d, 1);
}

template<class T>
void PlottingGenerator<T>::addPix(double x, double y, double r, double g, double b) {
	int x0 = (int) floor(x);
	int y0 = (int) floor(y);

	double degX = x - x0, degY = y - y0;

	double d00 = (1 - degX) * (1 - degY);
	double d10 = degX * (1 - degY);
	double d01 = (1 - degX) * degY;
	double d11 = degX * degY;

	addPix(x0, y0, r, g, b, d00);
	addPix(x0, y0 + 1, r, g, b, d01);
	addPix(x0 + 1, y0, r, g, b, d10);
	addPix(x0 + 1, y0 + 1, r, g, b, d11);
}

template<class T>
void PlottingGenerator<T>::addPix(int x, int y, double r, double g, double b, double density) {
	if(x >= 0 && y >= 0 && x < this->width() && y < this->height() && density > 0) {
		int index = x + this->width() * y;

		double d0 = density_[index];
		double r0 = r_[index];
		double g0 = g_[index];
		double b0 = b_[index];

		double dt = d0 + density;

		r_[index] = r0 * d0 / dt + r * density / dt;
		g_[index] = g0 * d0 / dt + g * density / dt;
		b_[index] = b0 * d0 / dt + b * density / dt;

		density_[index] = dt;

		if(dt > maxDensity_) {
			maxDensity_ = dt;
		}

		updatePix(x, y);
	}
}


template class Plotting<double>;
template class PlottingGenerator<double>;


template class Plotting<long double>;
template class PlottingGenerator<long double>;
