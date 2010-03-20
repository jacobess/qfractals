#include "mandelbrot.h"
#include "math.h"

//#include "math/interpreter.h"

#define SQR 0x8
#define ADD_C 0x9

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
	ops[0] = SQR << 26;
	ops[1] = ADD_C << 26;

	T regs[0x10 * 2];

	T nr, ni;
	T src2r, src2i, t;

	nr = zr;
	ni = zi;

	for(int i = 1; i < 10240; i++) {

		for(int j = 0; j < count; j++) {
			unsigned int op = ops[j];

			// Instruction Format
			// instruction [31...26] destination [25..18] source2/i [17..9] source [8..0]

			// Instructions are:
			// binary: + - * / ^ {if it is a binary function, source2 is also a source}
			// unary: sqr, lambda, sqrt, sin cos tan atan exp log conj 1/ *z *c +z +c, rect, polar
			// unary with number i encoded: ^int
			// const with number i encoded: z[n-i] cp[i] {i > 0}
			// const: z, pixel, c {in julia sets constant, in mandelbrot = pixel}

			// Sources are:
			// bit 8 = 1: Numbers {[7..4] + i * [3..0]}
			// else Registers: 0 = no register, use nr/ni

			// Instruction: [5..0]:
			// if < 8 [bits 5..3 unset]: src2 = source

			if(op & 0x1ff) { // if it is 0 we keep nr/ni
				// Macro to set nr/ni according to 0x1ff

				// SOURCE(nr, ni, op & 0x1ff, regs)

				unsigned int src1 = op & 0x1ff;
				if(src1) {
					// It's a number
					// Consider sign!
					nr = 0;
					ni = 0;
				} else {
					// It's a register (src1 in [1..255])
					//nr = regs[src1 - 1];
					//ni = regs[src1];
				}
			}

			if(op < (8 << 26)) {
				// SOURCE(src2r, src2i, op & 0x1ff, regs)
			}

			switch(op >> 26) {
			case SQR:
				t = nr * nr - ni * ni;
				ni = 2 * nr * ni;
				nr = t; break;
			case ADD_C:
				nr += cr;
				ni += ci;
				break;
			}

			if(op & (0xff << 18)) {
				unsigned int dest = (op << 18) & 0xff;

				regs[dest - 1] = nr;
				regs[dest] = ni;
			}
		}

		//INTERPRET(count, ops, nr, ni, src2r, src2i, cr, ci, zr, zi, xs_, ys_, i, regs, t)

		(*xn) = nr;
		(*yn) = ni;

		xn++;
		yn++;

		// check bailout
		T radSqr = nr * nr + ni * ni;

		if(radSqr > 256) {
			type = 1;

			// Smoothness
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
