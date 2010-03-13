#include "spline.h"

#include <math.h>

template<class T>
Spline<T>::Spline(T period) : period_(period) {}

template<class T>
T Spline<T>::mod(T x, int* pIndex) const {
	T p;

	if(period_ <= 0 || (0 <= x && x < period_)) {
		p = 0;
	} else {
		x /= period_;
		p = floor(x);
		x -= p;
		x *= period_;
	}

	if(pIndex != 0) {
		*pIndex = p;
	}

	return x;
}

template<class T>
int Spline<T>::modIndex(int i, int* pIndex) const {
	int p = T(i) / T(size());

	if(i < 0) {
		p --;
	}

	if(pIndex != 0) {
		*pIndex = p;
	}

	return i - size() * p;
}



template<class T>
T Spline<T>::operator()(T tx) const {
	if(isEmpty()) return 0;

	int l, r;

	tx = mod(tx);

	find(tx, l, r);

	if((l == r) || (period_ <= 0 && l < 0)) {
		return ys[r];
	} else if(period_ <= 0 && r >= size()) {
		return ys[l];
	} else {
		T x0 = x(l);
		T x1 = x(r);

		T d = (tx - x0) / (x1 - x0);

		return y(l) * (1 - d) + y(r) * d;
	}
}

template<class T>
T Spline<T>::x(int i) const {
	int p;
	i = modIndex(i, &p);
	return xs[i] + p * period_;
}

template<class T>
T Spline<T>::y(int i) const {
	return ys[modIndex(i)];
}

template<class T>
bool Spline<T>::isEmpty() const {
	return xs.isEmpty();
}

template<class T>
int Spline<T>::size() const {
	return xs.size();
}

template<class T>
int Spline<T>::add(T x, T y) {

	x = mod(x);

	if(isEmpty()) {
		xs.resize(1);
		ys.resize(1);

		xs[0] = x;
		ys[0] = y;

		return 0;
	} else {
		int l, r;
		find(x, l, r);

		if(l == r) {
			xs[l] = x;
			ys[l] = y;

			return l;
		} else {
			int s = size();

			xs.resize(s + 1);
			ys.resize(s + 1);

			// insert

			for(int i = s; i > r; i--) {
				xs[i] = xs[i - 1];
				ys[i] = ys[i - 1];
			}

			xs[r] = x;
			ys[r] = y;

			return r;
		}
	}
}

template<class T>
int Spline<T>::set(int i, T x, T y) {
	erase(i);
	return add(x, y);
}

template<class T>
T Spline<T>::erase(int i) {
	int s = size();

	T retVal = xs[i];

	for(int j = i; j < s - 1; j++) {
		xs[j] = xs[j + 1];
		ys[j] = ys[j + 1];
	}

	xs.resize(s - 1);
	ys.resize(s - 1);

	return retVal;
}

template<class T>
void Spline<T>::find(T x, int& left, int& right) const {
	int r = size() - 1;

	if(xs[r] < x) {
		left = r;
		right = r + 1;
		return;
	} else if(x < xs[0]) {
		left = -1;
		right = 0;
		return;
	}

	int l = 0;

	while(l + 1 < r) {
		// TODO Weighten
		int m = (l + r) / 2;

		if(x < xs[m]) r = m;
		else if(x > xs[m]) l = m;
		else l = r = m;
	}

	left = l;
	right = r;
}

template<class T>
void Spline<T>::setPeriod(T period) {
	int r = size();

	while(r > 0 && xs[r - 1] >= period) {
		r--;
	}

	period_ = period;

	xs.resize(r);
	ys.resize(r);
}

template class Spline<float>;
template class Spline<qreal>;
template class Spline<long double>;
