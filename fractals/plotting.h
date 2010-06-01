#ifndef PLOTTING_H
#define PLOTTING_H

#include "viewport.h"
#include "graphics/plottedimage.h"

template<class T>
class PlottingEnv {
public:
	virtual void next(T& x, T& y, double& r, double& g, double& b) = 0;
};

template<class T>
class Plotting : public Viewport<T> {
public:
	Plotting(const Transformation<T>& t);
	virtual ~Plotting();

	virtual Generator* createGenerator(int width, int height) const = 0;
	virtual PlottingEnv<T>* createEnv() const = 0;
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

	void exec(int index);
};

#endif // PLOTGENERATOR_H
