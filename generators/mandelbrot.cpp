#include <math.h>
#include "mandelbrot.h"

template<class T>
Mandelbrot<T>::Mandelbrot(const Transformation<T>& t,
	   const T& bailout,
	   int maxIterations,
	   const ColorPalette& palette) :
		Viewport<T>(t),
		bailout_(bailout),
		maxIterations_(maxIterations),
		palette_(palette) {}

template<class T>
const T& Mandelbrot<T>::bailout() const {
	return bailout_;
}

template<class T>
int Mandelbrot<T>::maxIterations() const {
	return maxIterations_;
}


template<class T>
void Mandelbrot<T>::color(unsigned char type, float val, float minVal, float maxVal,
			  float& r, float& g, float& b, float& a) const {

}

template<class T>
MandelbrotEnv<T>* Mandelbrot<T>::createEnv() const {
	return new MandelbrotEnv<T>(*this);
}

template<class T>
MandelbrotEnv<T>::MandelbrotEnv(const Mandelbrot<T>& parent) :
		RenderEnv(), parent_(parent) {}


template<class T>
void MandelbrotEnv<T>::calcPix(qreal x, qreal y, uchar &type, float &value) {
	T cr = parent_.transformation().toX(x, y);
	T ci = parent_.transformation().toY(x, y);

	T zr = cr, zi = ci;
	T zrSqr = 0, ziSqr = 0;

	for(int i = 0; i < parent_.maxIterations(); i++) {

		// TODO: Check fractal if no t

		T t = zrSqr - ziSqr + cr;
		zi = 2 * zr * zi + ci;
		zr = t;

		zrSqr = zr * zr;
		ziSqr = zi * zi;

		if(zrSqr + ziSqr > 4) {
			type = 1;
			value = i;

			return;
		}
	}

	type = 0;
	value = 0;
}

template<class T>
MandelbrotGenerator<T>::MandelbrotGenerator(int width, int height, int aaDeg,
					    const Mandelbrot<T>& mb) :
		specification_(mb),
		RenderGenerator(width, height, aaDeg, specification_) {
}


template class Mandelbrot<qreal>;
template class Mandelbrot<long double>;
template class MandelbrotEnv<qreal>;
template class MandelbrotEnv<long double>;
template class MandelbrotGenerator<qreal>;
template class MandelbrotGenerator<long double>;
