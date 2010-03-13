#ifndef COMPLEX_H
#define COMPLEX_H

template<class T>
class Complex
{
	T r_;
	T i_;

public:
	Complex(const T& r = 0, const T& i = 0) : r_(r), i_(i) {}

	const T& r() const { return r_; }
	T& r() { return r_; }

	const T& i() const { return i_; }
	T& i() { return i_; }

	Complex<T>& operator+=(const Complex<T>& c)
	{
		r_ += c.r_;
		i_ += c.i_;
	}
};

#endif // COMPLEX_H
