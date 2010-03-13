#ifndef ABSTRACTIMAGEGENERATOR_H
#define ABSTRACTIMAGEGENERATOR_H

#include "imagegenerator.h"
#include "math/transformation.h"
#include "graphics/image.h"

template<class T>
class Viewport {
	Transformation<T> t_;
public:
	Viewport(const Transformation<T>& t);

	const Transformation<T>& transformation() const;
	Transformation<T>& transformation();
};

template<class T>
class ViewportGenerator : public SelectableGenerator {
public:
	void scale(int cx, int cy, qreal factor);
	void move(int dx, int dy);
	void select(qreal wx, qreal wy, qreal hx, qreal hy, qreal x0, qreal y0);

	virtual const Viewport<T>& specification() const = 0;

protected:
	virtual Viewport<T>& specification() = 0;
};

#endif // ABSTRACTIMAGEGENERATOR_H
