#include "plotting.h"

template<class T>
Plotting<T>::Plotting(const Transformation<T>& t) :
	Viewport<T>(t) {}


template<class T>
Plotting<T>::~Plotting() {}

template<class T>
PlottingGenerator<T>::PlottingGenerator(int width, int height) :
		ViewportGenerator<double>(-1),
		img_(width, height) {}


template<class T>
PlottedImage& PlottingGenerator<T>::img() {
	return img_;
}


template<class T>
const PlottedImage& PlottingGenerator<T>::img() const {
	return img_;
}

template<class  T>
int PlottingGenerator<T>::progress() const {
	return 0;
}

template<class  T>
int PlottingGenerator<T>::totalSteps() const {
	return 0;
}

template<class T>
void PlottingGenerator<T>::init() {
	img_.init();
}


template<class T>
void PlottingGenerator<T>::exec(int) {
	PlottingEnv<T>* env = specification().createEnv();

	while(!this->isAborted()) {
		T x, y;
		double r, g, b;

		env->next(x, y, r, g, b);

		T tx = specification().transformation().fromX(x, y);
		T ty = specification().transformation().fromY(x, y);

		double x0 = this->denormX(tx);
		double y0 = this->denormY(ty);

		img_.addDot(x0, y0, r, g, b);
	}

	delete env;
}

template class Plotting<double>;
template class PlottingGenerator<double>;
template class PlottingEnv<double>;
