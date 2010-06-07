#ifndef ORBITPLOTTING_H
#define ORBITPLOTTING_H

#include <QList>
#include <QColor>

#include "plotting.h"

template<class T>
class OrbitPlottingEnv {
public:
	virtual bool orbit(T& x, T& y, double& r, double& g, double& b) = 0;

	virtual int length() const = 0;
	virtual const T& x(int i) const = 0;
	virtual const T& y(int i) const = 0;
};


template<class T>
class OrbitPlottable {
public:
	virtual OrbitPlottingEnv<T>* createEnv() const = 0;
};


template<class T>
class OrbitPlotting : public Plotting<T> {
	const OrbitPlottable<T>& spec_;
public:
	OrbitPlotting(const Transformation<T> &t, const OrbitPlottable<T>& spec);
	OrbitPlottingEnv<T>* createEnv() const;
	Generator* createGenerator(int width, int height) const;
};


template<class T>
class OrbitPlottingGenerator : public PlottingGenerator<T> {
	OrbitPlotting<T> spec_;

public:
	OrbitPlottingGenerator(int width, int height, const OrbitPlotting<T>& spec);
	const OrbitPlotting<T>& specification() const;

protected:
	void exec(int);

	OrbitPlotting<T>& spec();
};

#endif // ORBITPLOTTING_H
