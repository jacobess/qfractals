#ifndef IFS_H
#define IFS_H

#include <QList>
#include <QColor>

#include "plotting.h"

template<class T>
class IFSEnv;

template<class T>
class IFS : public Plotting<T> {

	QList<double> probs_;
	double totalProbs_;
	QList< Transformation<T> > affs_;
	QList<QColor> colors_;

public:
	IFS(const Transformation<T>& t,
	    const QList< Transformation<T> >& affs,
	    const QList<QColor>& colors);

	Generator* createGenerator(int width, int height) const;
	IFSEnv<T>* createEnv() const;

friend class IFSEnv<T>;
};

template<class T>
class IFSEnv : public PlottingEnv<T> {
	const IFS<T>& ifs_;

	T x_;
	T y_;

	double r_, g_, b_;

public:
	IFSEnv(const IFS<T>& ifs);
	void next(T& x, T& y, double& r, double& g, double& b);
};

template<class T>
class IFSGenerator : public PlottingGenerator<T> {
	IFS<T> spec_;

public:
	IFSGenerator(int width, int height, const IFS<T>& spec);
	const IFS<T>& specification() const;

protected:
	IFS<T>& spec();
};

#endif // IFS_H
