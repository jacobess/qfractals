#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <QVarLengthArray>

#include "rendergenerator.h"
#include "plotgenerator.h"
#include "viewportimagegenerator.h"
#include "math/complex.h"
#include "math/transformation.h"
#include "graphics/colorpalette.h"

template<class T>
class MandelbrotEnv;

template<class T>
class MandelbrotGenerator;

template<class T>
class BuddhabrotGenerator;

template<class T>
class Mandelbrot : public Viewport<T>, public RenderSpec, public ColorProvider {

	T bailout_;
	int maxIterations_;
	ColorPalette palette_;

public:
	Mandelbrot(const Transformation<T>& t,
		   const T& bailout,
		   int maxIterations,
		   const ColorPalette& palette);

	const T& bailout() const;

	int maxIterations() const;

	void color(uchar type, float val, float minVal, float maxVal,
		   float& r, float& g, float& b, float& a) const;

	MandelbrotEnv<T>* createEnv() const;
};

template<class T>
class MandelbrotEnv : public RenderEnv {
	const Mandelbrot<T>& parent_;

	QVarLengthArray<Complex<T> > orbit_;

public:
	MandelbrotEnv(const Mandelbrot<T>& parent);

	void calcPix(qreal x, qreal y, uchar& type, float& value);

	const QVarLengthArray<Complex<T> >& orbit();
};

template<class T>
class MandelbrotGenerator : public RenderGenerator, public ViewportGenerator<T> {
	Mandelbrot<T> specification_;

public:
	MandelbrotGenerator(int width, int height, int aaDeg,
			    const Mandelbrot<T>& mandelbrot);

	const Mandelbrot<T>& specification() const;

protected:
	Mandelbrot<T>& specification();
};

#endif // MANDELBROT_H
