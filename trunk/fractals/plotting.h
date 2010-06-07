#ifndef PLOTTING_H
#define PLOTTING_H

#include "viewport.h"
#include "graphics/plottedimage.h"

template<class T>
class Plotting : public Viewport<T> {
public:
	Plotting(const Transformation<T>& t);
	virtual ~Plotting();

	virtual Generator* createGenerator(int width, int height) const = 0;
};

template<class T>
class PlottingGenerator : public ViewportGenerator<T> {

	PlottedImage img_;

public:
	PlottingGenerator(int width, int height);
	virtual const Plotting<T>& specification() const = 0;

	int progress() const;
	int totalSteps() const;

protected:
	PlottedImage& img();
	const PlottedImage& img() const;

	virtual Plotting<T>& spec() = 0;

	void init();

	double addDot(const T& x, const T& y, double r, double g, double b);
};

#endif // PLOTGENERATOR_H
