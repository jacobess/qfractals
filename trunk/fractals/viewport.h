#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "specification.h"
#include "math/transformation.h"

class ViewportProxy : public ImageGenerator {
	// pure interface containing the
	// method definitions required for SelectableWidget

public:
	ViewportProxy(int threadCount, int width, int height);
	virtual ~ViewportProxy();

	void scale(int cx, int cy, double factor);
	void move(int dx, int dy);
	void select(double wx, double wy, double hx, double hy, double x0, double y0);

protected:
	virtual void scaleUnsafe(int cx, int cy, double factor) = 0;
	virtual void moveUnsafe(int dx, int dy) = 0;
	virtual void selectUnsafe(double wx, double wy, double hx, double hy, double x0, double y0) = 0;
};

template<class T>
class Viewport : public Specification {
	Transformation<T> t_;
public:
	Viewport(const Transformation<T>& t);
	virtual ~Viewport();

	const Transformation<T>& transformation() const;
	Transformation<T>& transformation();

	virtual ImageGenerator* createGenerator(int width, int height) const= 0;
};

template<class T>
class ViewportGenerator : public ViewportProxy {
public:
	ViewportGenerator(int threadCount, int width, int height);
	virtual ~ViewportGenerator();

	virtual const Viewport<T>& specification() const = 0;

protected:
	virtual Viewport<T>& spec() = 0;

	virtual void scaleUnsafe(int cx, int cy, double factor);
	virtual void moveUnsafe(int dx, int dy);
	virtual void selectUnsafe(double wx, double wy, double hx, double hy, double x0, double y0);
};

#endif // VIEWPORT_H
