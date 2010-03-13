#ifndef MANDELBROT_H
#define MANDELBROT_H

#include "rendering.h"
#include "graphics/colorpalette.h"

template<class T>
class MandelbrotGenerator;

template<class T>
class MandelbrotEnv;

template<class T>
class Mandelbrot : public Rendering<T>, public ColorProvider
{
	ColorPalette palette_;
public:
	Mandelbrot(const Transformation<T>& t, const ColorPalette& palette);
	virtual ~Mandelbrot();

	void color(uchar type, float val,
		   float& r, float& g, float& b, float& a) const;

	RenderingGenerator<T>* createGenerator(int width, int height) const;

	RenderingEnv<T>* createEnv() const;
};

template<class T>
class MandelbrotEnv : public RenderingEnv<T> {
	const Mandelbrot<T>& spec_;
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
