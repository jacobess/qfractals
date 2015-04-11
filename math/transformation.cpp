#include "transformation.h"

template <class T>
Transformation<T>::Transformation(const T& a, const T& b, const T& c, const T& d, const T& e, const T& f) :
	a_(a), b_(b), c_(c), d_(d), e_(e), f_(f), det_(a * d - b * c) {}

template <class T>
Transformation<T> Transformation<T>::operator* (const Transformation<T>& t) const {

	T na = a_ * t.a_ + c_ * t.b_;
	T nc = a_ * t.c_ + c_ * t.d_;

	T nb = b_ * t.a_ + d_ * t.b_;
	T nd = b_ * t.c_ + d_ * t.d_;

	T ne = e_ * t.a_ + f_ * t.b_ + t.e_;
	T nf = e_ * t.c_ + f_ * t.d_ + t.f_;

	return Transformation<T>(na, nb, nc, nd, ne, nf);
}

template <class T>
const T& Transformation<T>::det() const {
	return det_;
}

template <class T>
T Transformation<T>::toX(const T& x, const T& y) const {
	return a_ * x + b_ * y + e_;
}

template <class T>
T Transformation<T>::toY(const T& x, const T& y) const {
    return c_ * x + d_ * y + f_;
}

template <class T>
T Transformation<T>::fromX(const T& x, const T& y) const {
	return T(d_ / det_ * x + -b_ / det_ * y + (b_ * f_ - d_ * e_) / det_);
}

template <class T>
T Transformation<T>::fromY(const T& x, const T& y) const {
    return T(-c_ / det_ * x + a_ / det_ * y + (c_ * e_ - a_ * f_) / det_);
}

template class Transformation<float>;
template class Transformation<double>;
template class Transformation<long double>;
