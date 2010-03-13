#include <math.h>

#include "magneticpendulum.h"


template<class T>
MagneticEnv<T>::MagneticEnv(const Magnetic<T> &parent) :
	parent_(parent) {}


template<class T>
void MagneticEnv<T>::calc(const T& x, const T& y, uchar& type, float& value) {
	// Step size
	T dt = parent_stepSize;

	T abortVelSqr = parent->abortVelocity * parent->abortVelocity;
	T abortRadSqr = parent->abortRadius * parent->abortRadius;

	T heightSqr = parent->height * parent->height;

	vel[0] = vel[1] = acc0[0] = acc0[1] = acc1[0] = acc1[1] = 0;

	pos[0] = x;
	pos[1] = y;

	T distSqr;

	value = 0;

	for (int i = 0; i < parent->maxIterations; i++) {

		acc2[0] = 0;
		acc2[1] = 0;

		for(int j = 0; j < parent->magnets.size(); j++) {
			d[0] = parent->magnets[j].x() - pos[0];
			d[1] = parent->magnets[j].y() - pos[1];

			distSqr = d[0] * d[0] + d[1] * d[1];

			if(i >= parent->minIterations &&
			   distSqr < abortRadSqr) {
				// No!
				// is the velocity small enough?
				T v = vel[0] * vel[0] + vel[1] * vel[1];

				if(v < abortVelSqr) {
					type = j;
					// TODO Approximate value
					//value += sqrt(distSqr);
					return;
				}
			}

			// TODO: Abort-condition:
			// if |acc| < |acc_min| and
			// |vel| < |vel_min| abort and use
			// color of strongest impact on |acc| (including gravity)
			// Also consider statistical values of acc-impart of
			// different sources and gravity
			T dist = sqrt(distSqr + heightSqr);

			acc2[0] += parent->strengths[j] * d[0] / (dist * dist * dist);
			acc2[1] += parent->strengths[j] * d[1] / (dist * dist * dist);
		}

		acc2[0] -= vel[0] * parent->friction;
		acc2[1] -= vel[1] * parent->friction;

		acc2[0] -= pos[0] * parent->pullBackForce;
		acc2[1] -= pos[1] * parent->pullBackForce;

		vel[0] += dt * (1. / 3. * acc2[0] + 5. / 6. * acc1[0] - 1. / 6. * acc0[0]);
		vel[1] += dt * (1. / 3. * acc2[1] + 5. / 6. * acc1[1] - 1. / 6. * acc0[1]);

		acc0[0] = acc1[0];
		acc0[1] = acc1[1];

		acc1[0] = acc2[0];
		acc1[1] = acc2[1];

		// Get new position

		T dx = vel[0] * dt + dt * dt * (2. / 3. * acc1[0] - 1. / 6. * acc0[0]);
		T dy = vel[1] * dt + dt * dt * (2. / 3. * acc1[1] - 1. / 6. * acc0[1]);

		pos[0] += dx;
		pos[1] += dy;

		value += sqrt(vel[0] * vel[0] + vel[1] * vel[1]);
	}

	type = parent->magnets.size();
}

template<class T>
RenderGenerator<T>* MagneticEnv<T>::getParent() {
	return parent;
}


template<class T>
MagneticPendulum<T>::MagneticPendulum(int width, int height, int aaDeg, const Transformation<T>& t,
				      const QList<QPointF>& magnets, const QList<qreal>& strengths,
				      const QList<QColor>& colors, const T& friction, const T& h,
				      const T& stepSize, const T& pullBackForce,
				      const T& abortVelocity, const T& abortRadius,
				      int minIterations, int maxIterations):
		RenderGenerator<T>(width, height, aaDeg, t, 0),
		magnets(magnets), strengths(strengths), colors(colors),
		friction(friction), height(h), stepSize(stepSize), pullBackForce(pullBackForce),
		abortVelocity(abortVelocity), abortRadius(abortRadius), minIterations(minIterations),
		maxIterations(maxIterations) {
	this->setColorProvider();
}


template<class T>
RenderEnv<T>* MagneticPendulum<T>::create(int index, int count) {
	return new MagneticEnv<T>(this, index, count);
}



void MagneticPendulumColors::getColor(unsigned char type, float val, float minVal, float maxVal,
				   float& r, float& g, float& b, float& a) const {
	a = 1;
	if(type < magnets.size()) {
		//d = 1 (minVal)
		//d = 0.5 (maxVal)
		float d;

		if(minVal == maxVal) d = 1;
		else d = 1 - (val - minVal) / (maxVal - minVal);

		d = 1. / exp((log(256/10)/(maxVal*maxVal))*(val*val));

		r = colors[type].redF() * d;
		g = colors[type].greenF() * d;
		b = colors[type].blueF() * d;
	} else {
		r = g = b = 0;
	}
}

template class MagneticPendulum<float>;
template class MagneticPendulum<double>;
template class MagneticPendulum<long double>;
template class MagneticEnv<float>;
template class MagneticEnv<double>;
template class MagneticEnv<long double>;
