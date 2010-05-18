#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "specification.h"
#include "math/transformation.h"

template<class T>
class Viewport : public Specification {
	Transformation<T> t_;
public:
	Viewport(const Transformation<T>& t);
	virtual ~Viewport();

	const Transformation<T>& transformation() const;
	Transformation<T>& transformation();

	virtual Generator* createGenerator(int width, int height) const= 0;
};

template<class T>
class ViewportGenerator : public Generator {
public:
	ViewportGenerator(int threadCount);
	virtual ~ViewportGenerator();

	void scale(int cx, int cy, double factor);
	void move(int dx, int dy);
	void select(double wx, double wy, double hx, double hy, double x0, double y0);

	virtual const Viewport<T>& specification() const = 0;

	QString pointDescription(double x, double y);

protected:
	virtual Viewport<T>& specification() = 0;
};

#endif // VIEWPORT_H
