#include "viewportimagegenerator.h"

template<class T>
void ViewportGenerator<T>::scale(int cx, int cy, qreal factor) {
	QMutexLocker locker(&mutex);
	cancelAndWaitUnsafe();

	img().scale(cx, cy, factor);

	qreal dx = 0.5 - img().normX(cx);
	qreal dy = 0.5 - img().normY(cy);

	Transformation<T> s(factor, 0, 0, factor, (1 - factor) * dx, (1 - factor) * dy);

	Transformation<T>& t = specification().transformation();
	t = s * t;

	startUnsafe();
}

template<class T>
void ViewportGenerator<T>::move(int dx, int dy) {
	QMutexLocker locker(&mutex);
	cancelAndWaitUnsafe();

	img().move(dx, dy);

	int min = width() < height() ? width() : height();

	T dx0 = T(-dx) / T(min);
	T dy0 = T(-dy) / T(min);

	Transformation<T> tr(1, 0, 0, 1, dx0, dy0);

	Transformation<T>& t = specification().transformation();
	t = tr * t;

	startUnsafe();
}

template<class T>
void ViewportGenerator<T>::select(qreal wx, qreal wy, qreal hx, qreal hy, qreal x0, qreal y0) {
	QMutexLocker locker(&mutex);
	cancelAndWaitUnsafe();

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

	Transformation<T>& t = specification().transformation();
	t = s * t;

	startUnsafe();
}

template class Viewport<qreal>;
template class Viewport<long double>;

template class ViewportGenerator<qreal>;
template class ViewportGenerator<long double>;
