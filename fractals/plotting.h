#ifndef PLOTTING_H
#define PLOTTING_H

#include <QVarLengthArray>

#include "viewport.h"

template<class T>
class Plotting : public Viewport<T> {
public:
	Plotting(const Transformation<T>& t);
	virtual ~Plotting();

	virtual ImageGenerator* createGenerator(int width, int height) const = 0;
};

template<class T>
class PlottingGenerator : public ViewportGenerator<T> {

	double maxDensity_;

	QVarLengthArray<double> r_;
	QVarLengthArray<double> g_;
	QVarLengthArray<double> b_;
	QVarLengthArray<double> density_;

public:
	PlottingGenerator(int width, int height);
	virtual ~PlottingGenerator();
	virtual const Plotting<T>& specification() const = 0;

	int progress() const;
	int totalSteps() const;

protected:
	virtual Plotting<T>& spec() = 0;

	void init();

	double addDot(const T& x, const T& y, double r, double g, double b);

	void clear();
	void updatePix(int x, int y);

	void addPix(double x, double y, double r, double g, double b);
	void addPix(int x, int y, double r, double g, double b, double density);

	void setSizeUnsafe(int w, int h);

	void scaleUnsafe(int cx, int cy, double factor);
	void moveUnsafe(int dx, int dy);
	void selectUnsafe(double wx, double wy, double hx, double hy, double x0, double y0);

	void refreshUnsafe();
};

#endif // PLOTGENERATOR_H
