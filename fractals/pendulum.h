#ifndef PENDULUM_H
#define PENDULUM_H

#include <QList>

#include "rendering.h"
#include "orbitplotting.h"
#include "graphics/colorpalette.h"

template<class T>
class Pendulum;

template<class T>
class Magnet
{
	T x_, y_;
	T strength_;
	ColorPalette palette_;

public:
	Magnet(const T& x_, const T& y_, const T& strength, const ColorPalette& palette);

	const T& x() const;
	T& x();

	const T& y() const;
	T& y();

        const T& strength() const;
	T& strength();

        const ColorPalette& palette() const;
	ColorPalette& palette();
};


template<class T>
class PendulumEnv : public RenderingEnv<T>, public OrbitPlottingEnv<T> {
	const Pendulum<T>& spec_;

	// Orbit
	int magnet_;

	T* xs_;
	T* ys_;

	int n_;
public:
	PendulumEnv(const Pendulum<T>& spec);
	virtual ~PendulumEnv();
	void calc(const T& x0, const T& y0, uchar& type, double& value);

	bool orbit(T& x, T& y, double& r, double& g, double& b);

	int length() const;
	const T& x(int i) const;
	const T& y(int i) const;
};


template<class T>
class Pendulum : public Rendering<T>, public OrbitPlottable<T>
{
	QList< Magnet<T> > magnets_;
	int maxStepCount_;
	T stepSize_;
	T friction_;
	T height_;
	T gravity_;

public:
	Pendulum(const Transformation<T>& t,
		 const QList< Magnet<T> >& magnets,
		 int maxStepCount,
		 const T& stepSize,
		 const T& friction,
		 const T& height,
		 const T& gravity);

	ImageGenerator* createGenerator(int width, int height) const;

	PendulumEnv<T>* createEnv() const;

	const QList< Magnet<T> >& magnets() const;
	QList< Magnet<T> >& magnets();

	int maxStepCount() const;
	int& maxStepCount();

	const T& stepSize() const;
	T& stepSize();

	const T& friction() const;
	T& friction();

	const T& height() const;
	T& height();

	const T& gravity() const;
	T& gravity();
};

template<class T>
class PendulumGenerator : public RenderingGenerator<T>
{
	Pendulum<T> spec_;
public:
	PendulumGenerator(int width, int height, const Pendulum<T>& spec);
	virtual ~PendulumGenerator();

	const Pendulum<T>& specification() const;

	void color(uchar type, double value,
		   double& r, double& g, double& b, double& a) const;

protected:
	Pendulum<T>& spec();
};

#endif // PENDULUM_H
