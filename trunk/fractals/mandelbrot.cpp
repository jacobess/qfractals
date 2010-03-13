#include "mandelbrot.h"
#include "math.h"

template<class T>
Mandelbrot<T>::Mandelbrot(const Transformation<T>& t, const ColorPalette& palette) :
		Rendering<T>(t), palette_(palette) {}

template<class T>
Mandelbrot<T>::~Mandelbrot() {}

template<class T>
void Mandelbrot<T>::color(uchar type, float val,
			  float& r, float& g, float& b, float& a) const {
	if(type == 1) {
		if(val > 2.718) {
			val /= log(val);
		}

		palette_.color(val, r, g, b, a);
	} else {
		r = g = b = 0;
		a = 1;
	}
}

template<class T>
RenderingGenerator<T>* Mandelbrot<T>::createGenerator(int width, int height) const {
	return new MandelbrotGenerator<T>(width, height, *this);
}

template<class T>
RenderingEnv<T>* Mandelbrot<T>::createEnv() const {
	return new MandelbrotEnv<T>(*this);
}

template<class T>
MandelbrotEnv<T>::MandelbrotEnv(const Mandelbrot<T>& spec) :
	spec_(spec) {}

template<class T>
MandelbrotEnv<T>::~MandelbrotEnv() {}

template<class T>
void MandelbrotEnv<T>::calc(const T& x, const T& y, uchar& type, float& value) {
	T zr = x, zi = y;
	T zrSqr = 0, ziSqr = 0;

	for(int i = 0; i < 10240; i++) {
		zrSqr = zr * zr;
		ziSqr = zi * zi;

		T rad = zrSqr + ziSqr;

		if(rad > 256) {
			type = 1;

			// Smoothness

			value = i + 1 + 1 / log(2) * log(log(256) / log(rad));

			return;
		}

		zi = 2 * zr * zi + y;
		zr = zrSqr - ziSqr + x;
	}

	value = 0;
	type = 0;
}

template<class T>
MandelbrotGenerator<T>::MandelbrotGenerator(int width, int height, const Mandelbrot<T>& spec) :
		RenderingGenerator<T>(width, height, 2, &spec),
		spec_(spec) {
	this->img().setColorProvider(&spec_);
}

template<class T>
MandelbrotGenerator<T>::~MandelbrotGenerator() {}

template<class T>
const Mandelbrot<T>& MandelbrotGenerator<T>::specification() const {
	return spec_;
}

template<class T>
Mandelbrot<T>& MandelbrotGenerator<T>::specification() {
	return spec_;
}

template class Mandelbrot<qreal>;
template class MandelbrotEnv<qreal>;
template class MandelbrotGenerator<qreal>;

template class Mandelbrot<long double>;
template class MandelbrotEnv<long double>;
template class MandelbrotGenerator<long double>;
