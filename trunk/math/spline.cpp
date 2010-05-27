#include "spline.h"

Spline::Spline(double period) : period_(period) {}

int Spline::size() const {
	return x_.size();
}

void Spline::add(double x, double y) {
	x = mod(x);

	if(size() == 0) {
		x_.push_back(x);
		y_.push_back(y);
	} else {
		int l;
		int r;

		find(x, l, r);

		if(l == r) {
			y_[l] = y;
		} else {
			x_.insert(r, x);
			y_.insert(r, y);
		}

		initSpline();
	}
}

void Spline::setPeriod(double period) {
	period_ = period;

	if(period < INFINITY) {
		while(!x_.isEmpty() && x_.last() > period) {
			x_.removeLast();
			y_.removeLast();
		}

		while(!x_.isEmpty() && x_.first() < 0) {
			x_.removeFirst();
			y_.removeFirst();
		}
	}

	initSpline();
}


double Spline::operator()(double t) const {

	if(x_.isEmpty()) return 0;
	else if(size() == 1) return y_[0];

	int l, r;

	t = mod(t);

	find(t, l, r);

	if(l == r) {
		return y_[l];
	} else {
		if(period_ == INFINITY && l < 0) {
			return y_.first();
		} else if(period_ == INFINITY && r >= size()) {
			return y_.last();
		} else {
			t -= x(l);
			return ((a_[l] * t + b_[l]) * t + c_[l]) * t + y_[l];

			// Linear
			//double d = t / (x(l+1) - x(l));
			//return y_[l] * (1 - d) + y_[md(l + 1)] * d;
		}
	}
}

void Spline::find(double x, int& left, int& right) const {

	if(x_.last() < x) {
		left = size() - 1;
		right = size();
		return;
	} else if(x < x_.first()) {
		left = -1;
		right = 0;
		return;
	}

	int l = 0;
	int r = size() - 1;

	while(l + 1 < r) {
		int m = (l + r) / 2;

		if(x < x_.at(m)) r = m;
		else if(x > x_.at(m)) l = m;
		else l = r = m;
	}

	left = l;
	right = r;
}


double Spline::mod(double x) const {

	if(period_ < INFINITY) {
		if(!x_.isEmpty()) x -= x_[0];

		x /= period_;
		double p = floor(x);
		x -= p;
		x *= period_;

		if(!x_.isEmpty()) x += x_[0];
	}

	return x;
}

int Spline::md(int i) const {
	if(period_ < INFINITY) {
		i %= size();
		if(i < 0) i += size();
	}

	return i;
}


double Spline::x(int i) const {
	if(period_ < INFINITY) {
		int j = md(i);

		return x_[j] + ((i - j) / size()) * period_;
	} else {
		return x_[i];
	}
}


void Spline::initSpline() {
	if(size() > 1) {
		if(period_ < INFINITY) {
			a_.resize(size());
			b_.resize(size());
			c_.resize(size());

			int l0 = 0;

			// find smallest y
			for(int i = 1; i < size(); i++) {
				if(y_[i] < y_[l0]) l0 = i;
			}

			int l = l0;

			for(int r = l0 + 1; r < l0 + size(); r ++) {
				if((y_[md(r - 1)] - y_[md(r)]) * (y_[md(r)] - y_[md(r+1)]) <= 0) {
					createSubSpline(l, r);
					l = r;
				}

			}

			createSubSpline(l, size() + l0);
		} else {
			a_.resize(size() - 1);
			b_.resize(size() - 1);
			c_.resize(size() - 1);

			createSubSpline(0, size() - 1);
		}
	}
}

void Spline::createSubSpline(int l, int r) {
	int n = r - l;

	QVarLengthArray<double> a;
	QVarLengthArray<double> b;
	QVarLengthArray<double> c;
	QVarLengthArray<double> d;

	a.resize(n + 1);
	b.resize(n + 1);
	c.resize(n + 1);
	d.resize(n + 1);

	for(int i = 0; i <= n; i++) {
		d[i] = y_[md(i + l)];
	}

	QVarLengthArray<double> h;
	h.resize(n);

	for(int i = 0; i < n; i++) {
		h[i] = x(l + i + 1) - x(l + i);
	}

	QVarLengthArray<double> t;
	t.resize(n + 1);

	t[0] = 3 * (d[1] - d[0]) / h[0];
	t[n] = -3 * (d[n] - d[n - 1]) / h[r - l - 1];

	for(int i = 1; i < r - l; i++) {
		t[i] = 3 / h[i] * (d[i + 1] - d[i]) -
		       3 / h[i - 1] * (d[i] - d[i - 1]);
	}

	QVarLengthArray<double> m;
	m.resize(n + 1);

	QVarLengthArray<double> mu;
	mu.resize(n);

	QVarLengthArray<double> z;
	z.resize(n + 1);

	m[0] = 2 * h[0];
	mu[0] = 0.5;

	z[0] = t[0] / m[0];

	for(int i = 1; i < r - l; i++) {
		m[i] = 2 * (x(i + l + 1) - x(i + l - 1)) - h[i - 1] * mu[i - 1];
		mu[i] = h[i] / m[i];
		z[i] = (t[i] - h[i - 1] * z[i - 1]) / m[i];
	}

	m[n] = h[n-1] * (2 - mu[n-1]);
	b[n] = z[n] = (t[n] - h[n-1] * z[n-1]) / m[n];

	for(int i = n - 1; i >= 0; i--) {
		b[i] = z[i] - mu[i] * b[i + 1];

		c[i] = (d[i+1] - d[i]) / h[i] - h[i] * (b[i+1] + 2 * b[i]) / 3;

		a[i] = (b[i+1] - b[i]) / (3 * h[i]);
	}

	for(int i = 0; i < n; i++) {
		a_[md(i + l)] = a[i];
		b_[md(i + l)] = b[i];
		c_[md(i + l)] = c[i];
	}
}
