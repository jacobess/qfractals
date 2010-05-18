#ifndef SPLINE_H
#define SPLINE_H

#include<QVarLengthArray>


template<class T>
class Spline {
	T period_;

        QVarLengthArray<T> xs;
        QVarLengthArray<T> ys;

public:

	Spline(T period = 0);

	T mod(T x, int* pIndex = 0) const;

	void find(T x, int& l, int& r) const;

	T operator()(T x) const;

	T x(int i) const;
	T y(int i) const;

	int add(T x, T y);
	int set(int i, T x, T y);
	T erase(int i);
	int size() const;
	bool isEmpty() const;

	void setPeriod(T period);

protected:
	int modIndex(int i, int* pIndex = 0) const;
};

#endif // SPLINE_H
