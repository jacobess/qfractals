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
		qreal d = affs[i].det();

		if(d < 0) d = -d;

		totalProbs_ += d + 0.001;
		probs_.append(totalProbs_);
	}
}

template<class T>
IFSEnv<T>* IFS<T>::createEnv() const {
	return new IFSEnv<T>(*this);
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
IFSEnv<T>::IFSEnv(const IFS<T>& ifs) :
		ifs_(ifs),
		x_(0), y_(0),
		r_(0), g_(0), b_(0) {}

template<class T>
void IFSEnv<T>::next(T& x, T& y, double& r, double&g, double& b) {
	double rnd = double(rand()) * ifs_.totalProbs_ / RAND_MAX;

	int i = 0;

	while(ifs_.probs_[i] < rnd) i++;

	x = ifs_.affs_[i].toX(x_, y_);
	y = ifs_.affs_[i].toY(x_, y_);

	x_ = x;
	y_ = y;

	if(ifs_.colors_.size() == 0) {
		r = g = b = 1;
	} else if(i >= ifs_.colors_.size()) {
		r = r_;
		g = g_;
		b = b_;
	} else {
		double r0 = ifs_.colors_[i].redF();
		double g0 = ifs_.colors_[i].greenF();
		double b0 = ifs_.colors_[i].blueF();

		int lim = ifs_.colors_.size() < ifs_.affs_.size() ?
			   ifs_.colors_.size() : ifs_.affs_.size();

		double total = ifs_.probs_[lim - 1];

		double p = ifs_.probs_[i];
		if(i > 0) p -= ifs_.probs_[i - 1];

		double pc = total - p;

		p *= (lim - 1);

		r_ = r = (r_ * p + r0 * pc) / (p + pc);
		g_ = g = (g_ * p + g0 * pc) / (p + pc);
		b_ = b = (b_ * p + b0 * pc) / (p + pc);
	}
}

template class IFS<double>;
template class IFSGenerator<double>;
template class IFSEnv<double>;
