#include "mandelbrot.h"
#include "math.h"

#include "math/interpreter.h"

template<class T>
Mandelbrot<T>::Mandelbrot(const Transformation<T>& t, const ColorPalette& palette) :
		Rendering<T>(t), palette_(palette) {}

template<class T>
Mandelbrot<T>::~Mandelbrot() {}

template<class T>
void Mandelbrot<T>::color(uchar type, float val,
			  float& r, float& g, float& b, float& a) const {
	if(type == 1) {
		if(val > 2.7183) {
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
	spec_(spec), xs_(new T[10240]), ys_(new T[10240]) {}

template<class T>
MandelbrotEnv<T>::~MandelbrotEnv() {
	delete xs_;
	delete ys_;
}

template<class T>
void MandelbrotEnv<T>::calc(const T& x, const T& y, uchar& type, float& value) {

	/* Initialization phase */
	T cr = x;
	T ci = y;

	T zr = 0;
	T zi = 0;

	// Init orbit

	T* xn = xs_;
	T* yn = ys_;

	(*xn) = zr;
	(*yn) = zi;

	xn++;
	yn++;

	// Stack allocation - size hardly has impact

	unsigned int ops[16];

	int count = 2;
	ops[0] = OP_SQR << 26;
	ops[1] = OP_ADD_C << 26;

	T regs[0x10 * 2];

	T nr, ni;

	nr = zr;
	ni = zi;

	for(int i = 1; i < 10240; i++) {

		// For the sake of speed avoid pointer resolving and function calls
		// in here. Therefore here using a macro

		INTERPRET(count, ops, nr, ni, cr, ci, zr, zi, i, xs_, ys_, regs);

		//T t = nr * nr - ni * ni + cr;
		//ni = 2 * nr * ni + ci;
		//nr = t;

		(*xn) = nr;
		(*yn) = ni;

		xn++;
		yn++;

		// check bailout
		T radSqr = nr * nr + ni * ni;

		if(radSqr > 256) {
			type = 1;

			// Smoothness
			// TODO Replace 2 by power
			value = i + 1 + 1 / log(2) * log(log(256) / log(radSqr));
			n_ = i + 1;

			return;
		}

		// Check epsilon
		T dr = zr - nr;
		T di = zi - ni;

		T deltaSqr = dr * dr + di * di;

		if(deltaSqr < 1e-12) {
			n_ = i + 1;

			type = 0;
			value = 0;

			return;
		}

		zr = nr;
		zi = ni;
	}

	value = 0;
	type = 0;

	n_ = 10240;
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
