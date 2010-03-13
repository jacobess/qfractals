#include "plotgenerator.h"
#include "settings.h"


PlotGenerator::PlotGenerator(int width, int height) :
		plotImg_(width, height) {}


Image& PlotGenerator::img() {
	return plotImg_;
}


const Image& PlotGenerator::img() const {
	return plotImg_;
}


void PlotGenerator::init() {
	plotImg_.init();
}


void PlotGenerator::exec(int /*index*/, int /*count*/) {
	float r, g, b;

	PlotEnv* env = specification().createEnv();

	while(this->isRunning()) {
		qreal x, y;
		env->nextPix(x, y, r, g, b);

		plotImg_.addDot(x, y, r, g, b);
	}

	delete env;
}
