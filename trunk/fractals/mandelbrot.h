#ifndef MANDELBROT_H
#define MANDELBROT_H

#include "rendering.h"
#include "orbitplotting.h"
#include "graphics/colorpalette.h"

#include "math/interpreter.h"

template<class T>
class MandelbrotGenerator;

template<class T>
class Mandelbrot;

template<class T>
class MandelbrotEnv : public RenderingEnv<T>, public OrbitPlottingEnv<T> {
	const Mandelbrot<T>& spec_;

	// Orbit
	T* xs_;
	T* ys_;

	int n_;

public:
	MandelbrotEnv(const Mandelbrot<T>& spec);
	virtual ~MandelbrotEnv();

	void calc(const T& x, const T& y, uchar& type, double& value);

	bool orbit(T& x, T& y, double& r, double& g, double& b);

	int length() const;
	const T& x(int i) const;
	const T& y(int i) const;
};

template<class T>
class Mandelbrot : public Rendering<T>, public OrbitPlottable<T>
{
	QList< Interpreter<T> > base_;
	Interpreter<T> iteration_;

	int maxIterations_;

	T bailout_;
	T epsilon_;

	ColorPalette bailoutPalette_;
	ColorPalette epsilonPalette_;
public:
	Mandelbrot(const Transformation<T>& t,
		   const QList< Interpreter<T> >& base,
		   const Interpreter<T>& iteration,
		   int maxIterations,
		   const T& bailout,
		   const T& epsilon,
		   const ColorPalette& bailoutPalette,
		   const ColorPalette& epsilonPalette);
	virtual ~Mandelbrot();

	void color(uchar type, int count, double value,
		   double min, double max, double sum, double sqrSum,
		   double& r, double& g, double& b, double& a) const;

	RenderingGenerator<T>* createGenerator(int width, int height) const;

	MandelbrotEnv<T>* createEnv() const;

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

	const ColorPalette& epsilonPalette() const;
	ColorPalette& epsilonPalette();

friend class MandelbrotEnv<T>;
};



template<class T>
class MandelbrotGenerator : public RenderingGenerator<T>
{
	Mandelbrot<T> spec_;
public:
	MandelbrotGenerator(int width, int height, const Mandelbrot<T>& spec);
	virtual ~MandelbrotGenerator();

	const Mandelbrot<T>& specification() const;

	void color(uchar type, double value,
		   double& r, double& g, double& b, double& a) const;

protected:
	Mandelbrot<T>& spec();
};

#endif // MANDELBROT_H
