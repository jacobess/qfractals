#ifndef PLOTGENERATOR_H
#define PLOTGENERATOR_H

#include "imagegenerator.h"
#include "graphics/plottedimage.h"

class PlotEnv {
public:
	virtual void nextPix(qreal& x, qreal& y, float& r, float&g, float& b) = 0;
};

class PlotGenerator;

class PlotSpec {
public:
	virtual PlotEnv* createEnv() const = 0;
};

class PlotGenerator : public SelectableGenerator {

	PlottedImage plotImg_;

public:
	PlotGenerator(int width, int height);
	virtual const PlotSpec& specification() const = 0;

protected:
	Image& img();
	const Image& img() const;

	virtual PlotSpec& specification() = 0;

	void init();

	void exec(int index, int count);
};

#endif // PLOTGENERATOR_H
