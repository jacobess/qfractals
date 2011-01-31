#include "orbitplotting.h"

template<class T>
OrbitPlotting<T>::OrbitPlotting(const Transformation<T> &t, const OrbitPlottable<T>& spec) :
		Plotting<T>(t),
		spec_(spec) {}

template<class T>
ImageGenerator* OrbitPlotting<T>::createGenerator(int width, int height) const {
	return new OrbitPlottingGenerator<T>(width, height, *this);
}

template<class T>
OrbitPlottingEnv<T>* OrbitPlotting<T>::createEnv() const {
	return spec_.createEnv();
}

template<class T>
OrbitPlottingGenerator<T>::OrbitPlottingGenerator(int width, int height, const OrbitPlotting<T>& spec) :
		PlottingGenerator<T>(width, height),
		spec_(spec) {}

template<class T>
const OrbitPlotting<T>& OrbitPlottingGenerator<T>::specification() const {
	return spec_;
}

template<class T>
OrbitPlotting<T>& OrbitPlottingGenerator<T>::spec() {
	return spec_;
}

template<class T>
void OrbitPlottingGenerator<T>::exec(int) {
	OrbitPlottingEnv<T>* env = spec_.createEnv();

	while(!this->isTerminated()) {
		double min = -4;
		double max = 4;

		T x = min + (max - min) * rand() / RAND_MAX;
		T y = min + (max - min) * rand() / RAND_MAX;

		double r, g, b;

		if(env->orbit(x, y, r, g, b)) {

			for(int i = 0; i < env->length(); i++) {
				addDot(env->x(i), env->y(i), r, g, b);
			}
		}
	}
}


template class OrbitPlotting<double>;
template class OrbitPlottingEnv<double>;
template class OrbitPlottingGenerator<double>;

template class OrbitPlotting<long double>;
template class OrbitPlottingEnv<long double>;
template class OrbitPlottingGenerator<long double>;
