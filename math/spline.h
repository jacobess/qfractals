#ifndef SPLINE_H
#define SPLINE_H

#include <QList>
#include <QVarLengthArray>

#include <math.h>


class Spline {
	double period_;

	QList<double> x_;
	QList<double> y_;// == ys

	QVarLengthArray<double> a_;
	QVarLengthArray<double> b_;
	QVarLengthArray<double> c_;

public:

	Spline(double period = INFINITY);

	double operator()(double x) const;
	int size() const;

	void add(double x, double y);
	void setPeriod(double period);
private:
	void find(double x, int& left, int& right) const;
	double mod(double x) const;
	int md(int i) const;

	void initSpline();
	void createSubSpline(int l, int r);

	double x(int i) const;
};

#endif // SPLINE_H
