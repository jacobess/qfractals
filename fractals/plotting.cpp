#include "plotting.h"

template<class T>
Plotting<T>::Plotting(const Transformation<T>& t) :
	Viewport<T>(t) {}


template<class T>
Plotting<T>::~Plotting() {}

template<class T>
PlottingGenerator<T>::PlottingGenerator(int width, int height) :
		ViewportGenerator<T>(-1),
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
double PlottingGenerator<T>::addDot(const T &x, const T &y, double r, double g, double b) {
	T tx = specification().transformation().fromX(x, y);
	T ty = specification().transformation().fromY(x, y);

	double x0 = this->denormX(tx);
	double y0 = this->denormY(ty);

	double dx = x0 < 0 ? -x0 : this->width() - 1 < x0 ? x0 - this->width() + 1 : 1;

	double dy = y0 < 0 ? -y0 : this->height() - 1 < y0 ? y0 - this->height() + 1 : 1;

	if(dx <= 0 || dy <= 0) {
		return 0;
	} else {
		img_.addDot(x0, y0, r, g, b);
		return dx * dy;
	}
}

template class Plotting<double>;
template class PlottingGenerator<double>;


template class Plotting<long double>;
template class PlottingGenerator<long double>;
