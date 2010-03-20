#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <gmpxx.h>
#include "sys/types.h"

template <class T>
class Transformation
{
	T a_, b_, c_, d_, e_, f_, det_;
public:
	Transformation(const T& a = 1, const T& b = 0, const T& c = 0, const T& d = 1, const T& e = 0, const T& f = 0);

	Transformation<T> operator* (const Transformation<T>& t) const;
	Transformation<T>& operator*= (const Transformation<T>& t);
	const T& det() const;

	T toX(const T& x, const T& y) const;
	T toY(const T& y, const T& y) const;

	T fromX(const T& x, const T& y) const;
	T fromY(const T& y, const T& y) const;
};

#endif // TRANSFORMATION_H
