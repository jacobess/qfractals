#include "pendulum.h"

#include <math.h>

template<class T>
Magnet<T>::Magnet(const T& x0, const T& y0, const T& strength0, const ColorPalette& palette0) :
		x_(x0), y_(y0), strength_(strength0), palette_(palette0) {}

template<class T>
const T& Magnet<T>::x() const {
	return x_;
}

template<class T>
T& Magnet<T>::x() {
	return x_;
}

template<class T>
const T& Magnet<T>::y() const {
	return y_;
}

template<class T>
T& Magnet<T>::y() {
	return y_;
}

template<class T>
const T& Magnet<T>::strength() const {
	return strength_;
}

template<class T>
T& Magnet<T>::strength() {
	return strength_;
}

template<class T>
const ColorPalette& Magnet<T>::palette() const {
	return palette_;
}

template<class T>
ColorPalette& Magnet<T>::palette() {
	return palette_;
}

template<class T>
Pendulum<T>::Pendulum(const Transformation<T>& t,
                      const QList< Magnet<T> >& magnets,
		      int maxStepCount,
		      const T& stepSize,
		      const T& friction,
		      const T& height,
		      const T& gravity):
Rendering<T>(t),
magnets_(magnets),
maxStepCount_(maxStepCount),
stepSize_(stepSize),
friction_(friction),
height_(height),
gravity_(gravity) {}

template<class T>
Generator* Pendulum<T>::createGenerator(int width, int height) const {
	return new PendulumGenerator<T>(width, height, *this);
}

template<class T>
RenderingEnv<T>* Pendulum<T>::createEnv() const {
	return new PendulumEnv<T>(*this);
}

template<class T>
const QList< Magnet<T> >& Pendulum<T>::magnets() const {
	return magnets_;
}

template<class T>
QList< Magnet<T> >& Pendulum<T>::magnets() {
	return magnets_;
}

template<class T>
int Pendulum<T>::maxStepCount() const {
	return maxStepCount_;
}

template<class T>
int& Pendulum<T>::maxStepCount() {
	return maxStepCount_;
}

template<class T>
const T& Pendulum<T>::stepSize() const {
	return stepSize_;
}

template<class T>
T& Pendulum<T>::stepSize() {
	return stepSize_;
}

template<class T>
const T& Pendulum<T>::friction() const {
	return friction_;
}

template<class T>
T& Pendulum<T>::friction() {
	return friction_;
}

template<class T>
const T& Pendulum<T>::height() const {
	return height_;
}

template<class T>
T& Pendulum<T>::height() {
	return height_;
}

template<class T>
const T& Pendulum<T>::gravity() const {
	return gravity_;
}

template<class T>
T& Pendulum<T>::gravity() {
	return gravity_;
}

template<class T>
PendulumEnv<T>::PendulumEnv(const Pendulum<T>& spec) :
		spec_(spec) {}

template<class T>
PendulumEnv<T>::~PendulumEnv() {
	delete[] xs_;
	delete[] ys_;
}

template<class T>
void PendulumEnv<T>::calc(const T& x0, const T& y0, uchar& type, double& value) {
	// Step size
	T dt = 0.1;

	T height = spec_.height();
	T heightSqr = height * height;

	T gravity = spec_.gravity();

	T friction = spec_.friction();

	// Velocity
	T vx = 0;
	T vy = 0;

	// Acceleration
	T ax = 0;
	T ay = 0;

	// Previous acceleration
	T pax = 0;
	T pay = 0;

	// Next acceleration
	T nax = 0;
	T nay = 0;

	T x = x0;
	T y = y0;

	T total = 0;

	const QList< Magnet<T> >& magnets = spec_.magnets();

	for (int i = 0; i < spec_.maxStepCount(); i++) {
		T minDistSqr = x *  x + y * y;
		int minDistIndex = magnets.size();

		// Gravity
		nax -= x * gravity;
		nay -= y * gravity;

		// and magnets
		for(int j = 0; j < magnets.size(); j++) {
			const Magnet<T>& m = magnets[j];
			T dx = m.x() - x;
			T dy = m.y() - y;

			T distSqr = dx * dx + dy * dy;

			if(distSqr < minDistSqr) {
				minDistSqr = distSqr;
				minDistIndex = j;
			}

			distSqr += heightSqr;

			T forceX = m.strength() * dx / sqrt(distSqr * distSqr * distSqr);
			T forceY = m.strength() * dy / sqrt(distSqr * distSqr * distSqr);

			nax += forceX;
			nay += forceY;
		}

		nax -= vx * friction;
		nay -= vy * friction;

		vx += dt * (1. / 3. * nax + 5. / 6. * ax - 1. / 6. * pax);
		vy += dt * (1. / 3. * nay + 5. / 6. * ay - 1. / 6. * pay);

		T dx = vx * dt + dt * dt * (2. / 3. * nax - 1. / 6. * ax);
		T dy = vy * dt + dt * dt * (2. / 3. * nay - 1. / 6. * ay);

		x += dx;
		y += dy;

		// TODO Check end condition
		T speed = sqrt(dx * dx + dy * dy);
		total += speed;

                if((speed < 0.001 && minDistSqr < 0.001) || (i >= spec_.maxStepCount() - 1)) {
			type = minDistIndex;
			value = total / log(total + 1);

			return;
		}

		// Step ahead
		pax = ax;
		pay = ay;

		ax = nax;
		ay = nay;

		nax = 0;
		nay = 0;
	}
}

template<class T>
PendulumGenerator<T>::PendulumGenerator(int width, int height, const Pendulum<T>& spec):
		RenderingGenerator<T>(width, height, 2),
		spec_(spec) {}

template<class T>
PendulumGenerator<T>::~PendulumGenerator() {
}

template<class T>
Pendulum<T>& PendulumGenerator<T>::spec() {
	return  spec_;
}

template<class T>
const Pendulum<T>& PendulumGenerator<T>::specification() const {
	return  spec_;
}

template<class T>
void PendulumGenerator<T>::color(uchar type, double value,
			double& r, double& g, double& b, double& a) const {

	value -= this->min(type);
	value /= this->max(type) - this->min(type);

	value = value > 1 ? 1 : value < 0 ? 0 : value;

	if(type < spec_.magnets().size()) {
		spec_.magnets().at(type).palette().color(value, r, g, b, a);
	} else {
		// TODO: Should be used by gravity
		// White

		r = g = b = 1 - value;
		a = 1;
	}
}

template class Magnet<double>;
template class Magnet<long double>;

template class Pendulum<double>;
template class PendulumEnv<double>;
template class PendulumGenerator<double>;

template class Pendulum<long double>;
template class PendulumEnv<long double>;
template class PendulumGenerator<long double>;
