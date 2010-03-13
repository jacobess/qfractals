#include "ifs.h"

#include <math.h>



template<class T>
IFS<T>::IFS(const Transformation<T>& t,
	    const QList<Transformation<T> >& affTs,
	    const QList<QColor>& colors) :
		Viewport<T>(t),
		totalProbs_(0),
		affTs_(affTs),
		colors_(colors) {

	for(int i = 0; i < affTs.size(); i++) {
		qreal d = affTs[i].det();

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
IFSEnv<T>::IFSEnv(const IFS<T>& ifs) :
		ifs_(ifs),
		x_(0), y_(0),
		r_(0), g_(0), b_(0) {}

template<class T>
void IFSEnv<T>::nextPix(qreal& x, qreal& y, float& r, float&g, float& b) {
	/*qreal rnd = qreal(rand()) * ifs_.totalProbs / RAND_MAX;

	int i = 0;

	while(ifs_.probabilities[i] < rnd) i++;

	ifs->transformations[i].to(x_, y_, x, y);

	this->x = x;
	this->y = y;

	if(ifs->colors.size() == 0) {
		r = g = b = 1;
	} else if(i >= ifs->colors.size()) {
		r = this->r;
		g = this->g;
		b = this->b;
	} else {
		r = ifs->colors[i].redF();
		g = ifs->colors[i].greenF();
		b = ifs->colors[i].blueF();

		int lim = ifs->colors.size() < ifs->transformations.size() ?
			   ifs->colors.size() : ifs->transformations.size();

		float total = ifs->probabilities[lim - 1];

		float p = ifs->probabilities[i];
		if(i > 0) p -= ifs->probabilities[i - 1];

		float pc = total - p;

		p *= (lim - 1);

		// TODO Multiply according to probability
		r = this->r = (this->r * p + r * pc) / (p + pc);
		g = this->g = (this->g * p  + g * pc) / (p + pc);
		b = this->b = (this->b * p  + b * pc) / (p + pc);
	}*/
}

template class IFS<float>;
template class IFS<double>;
template class IFS<long double>;
template class IFSEnv<float>;
template class IFSEnv<double>;
template class IFSEnv<long double>;
