#ifndef MANDELBROT_H
#define MANDELBROT_H

#include "rendering.h"
#include "graphics/colorpalette.h"

#include "math/interpreter.h"

template<class T>
class MandelbrotGenerator;

template<class T>
class MandelbrotEnv;

template<class T>
class Mandelbrot : public Rendering<T>, public ColorProvider
{
	QList< Interpreter<T> > base_;
	Interpreter<T> iteration_;

	int maxIterations_;

	T bailout_;
	T epsilon_;

	ColorPalette bailoutPalette_;
public:
	Mandelbrot(const Transformation<T>& t,
		   const QList< Interpreter<T> >& base,
		   const Interpreter<T>& iteration,
		   int maxIterations,
		   const T& bailout,
		   const T& epsilon,
		   const ColorPalette& bailoutPalette);
	virtual ~Mandelbrot();

	void color(uchar type, float val,
		   float& r, float& g, float& b, float& a) const;

	RenderingGenerator<T>* createGenerator(int width, int height) const;

	RenderingEnv<T>* createEnv() const;

	const QList< Interpreter<T> >& base() const;
	QList< Interpreter<T> >& base();

	const Interpreter<T>& iteration() const;
	Interpreter<T>& iteration();

	int maxIterations() const;
	int& maxIterations();

	const T& epsilon() const;
	T& epsilon();

	const T& bailout() const;
	T& bailout();

	const ColorPalette& bailoutPalette() const;
	ColorPalette& bailoutPalette();
};

template<class T>
class MandelbrotEnv : public RenderingEnv<T> {
	const Mandelbrot<T>& spec_;

	// Orbit
	T* xs_;
	T* ys_;

	int n_;

public:
	MandelbrotEnv(const Mandelbrot<T>& spec);
	virtual ~MandelbrotEnv();
	void calc(const T& x, const T& y, uchar& type, float& value);
};

template<class T>
class MandelbrotGenerator : public RenderingGenerator<T>
{
	Mandelbrot<T> spec_;
public:
	MandelbrotGenerator(int width, int height, const Mandelbrot<T>& spec);
	virtual ~MandelbrotGenerator();

	const Mandelbrot<T>& specification() const;
protected:
	Mandelbrot<T>& specification();
};

#endif // MANDELBROT_H
