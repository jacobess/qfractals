#include "ifs.h"

#include <math.h>

template<class T>
IFS<T>::IFS(const Transformation<T>& t,
	    const QList<Transformation<T> >& affs,
	    const QList<QColor>& colors) :
		Plotting<T>(t),
		totalProbs_(0),
		affs_(affs),
		colors_(colors) {

	for(int i = 0; i < affs.size(); i++) {
		double d = affs[i].det();

		if(d < 0) d = -d;

		totalProbs_ += d + 0.001;
		probs_.append(totalProbs_);
	}
}

template<class T>
Generator* IFS<T>::createGenerator(int width, int height) const {
	return new IFSGenerator<T>(width, height, *this);
}

template<class T>
IFSGenerator<T>::IFSGenerator(int width, int height, const IFS<T> &spec) :
		PlottingGenerator<T>(width, height),
		spec_(spec) {}


template<class T>
const IFS<T>& IFSGenerator<T>::specification() const {
	return spec_;
}

template<class T>
IFS<T>& IFSGenerator<T>::spec() {
	return spec_;
}

template<class T>
void IFSGenerator<T>::exec(int) {
	T x = 0, y = 0;
	double r = 0, g = 0, b = 0;

	while(!this->isAborted()) {
		double rnd = double(rand()) * spec_.totalProbs_ / RAND_MAX;

		int i = 0;

		while(spec_.probs_[i] < rnd) i++;

		T x0 = x;
		T y0 = y;

		x = spec_.affs_[i].toX(x0, y0);
		y = spec_.affs_[i].toY(x0, y0);

		if(spec_.colors_.size() == 0) {
			r = g = b = 1;
		} else if(i < spec_.colors_.size()) {
			double r0 = spec_.colors_[i].redF();
			double g0 = spec_.colors_[i].greenF();
			double b0 = spec_.colors_[i].blueF();

			int lim = spec_.colors_.size() < spec_.affs_.size() ?
				   spec_.colors_.size() : spec_.affs_.size();

			double total = spec_.probs_[lim - 1];

			double p = spec_.probs_[i];
			if(i > 0) p -= spec_.probs_[i - 1];

			double pc = total - p;

			p *= (lim - 1);

			r = (r * p + r0 * pc) / (p + pc);
			g = (g * p + g0 * pc) / (p + pc);
			b = (b * p + b0 * pc) / (p + pc);
		}

		addDot(x, y, r, g, b);
	}
}

template class IFS<double>;
template class IFSGenerator<double>;
