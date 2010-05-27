#include "viewport.h"
#include "settings.h"
#include <sstream>

template<class T>
Viewport<T>::Viewport(const Transformation<T>& t) :
		t_(t) {}

template<class T>
Viewport<T>::~Viewport() {}

template<class T>
const Transformation<T>& Viewport<T>::transformation() const {
	return t_;
}

template<class T>
Transformation<T>& Viewport<T>::transformation() {
	return t_;
}

template<class T>
ViewportGenerator<T>::ViewportGenerator(int threadCount) :
		Generator(threadCount, true) {}

template<class T>
ViewportGenerator<T>::~ViewportGenerator() {}

template<class T>
void ViewportGenerator<T>::scale(int cx, int cy, double factor) {
	lockForCommit();

	img().scale(cx, cy, factor);

	double dx = img().normX(cx);
	double dy = img().normY(cy);

	Transformation<T> s(factor, 0, 0, factor, (1 - factor) * dx, (1 - factor) * dy);

	Transformation<T>& t = spec().transformation();
	t = s * t;

	finishCommit();
}

template<class T>
void ViewportGenerator<T>::move(int dx, int dy) {
	lockForCommit();

	img().move(dx, dy);

	int min = width() < height() ? width() : height();

	T dx0 = T(-dx) / T(min);
	T dy0 = T(-dy) / T(min);

	Transformation<T> tr(1, 0, 0, 1, dx0, dy0);

	Transformation<T>& t = spec().transformation();
	t = tr * t;

	finishCommit();
}

template<class T>
void ViewportGenerator<T>::select(double wx, double wy, double hx, double hy, double x0, double y0) {
	lockForCommit();

	img().select(wx, wy, hx, hy, x0, y0);


	// TODO Use norm and denorm
	int w = width();
	int h = height();

	x0 += (wx * w + hx * h) / 2;
	y0 += (wy * w + hy * h) / 2;

	if(w > h) {
		x0 -= (w - h) / 2.;

		x0 /= h;
		y0 /= h;
	} else {
		y0 -= (h - w) / 2.;

		x0 /= w;
		y0 /= w;
	}

	// And move origin back to upper left corner
	x0 -= (wx + hx) / 2.;
	y0 -= (wy + hy) / 2.;

	Transformation<T> s(wx, hx, wy, hy, x0, y0);

	Transformation<T>& t = spec().transformation();
	t = s * t;

	finishCommit();
}

template<class T>
QString ViewportGenerator<T>::pointDescription(double x, double y) {
	double x0 = img().normX(x);
	double y0 = img().normY(y);

	T tx = specification().transformation().toX(x0, y0);
	T ty = specification().transformation().toY(x0, y0);

	int prec = Settings::settings()->coordinatesPrecision();

	QString sx = QString("%1").arg((double) tx, 1, 'f', tx < 0 ? prec - 1 : prec);
	QString sy = QString("%1").arg((double) ty, 1, 'f', ty < 0 ? prec - 1 : prec);

	return QString("%1 | %2").arg(sx).arg(sy);
}


template class Viewport<double>;
template class ViewportGenerator<double>;

template class Viewport<long double>;
template class ViewportGenerator<long double>;
