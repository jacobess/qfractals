#ifndef IFS_H
#define IFS_H

#include <QList>
#include <QColor>

#include "plotting.h"

template<class T>
class IFSGenerator;

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

friend class IFSGenerator<T>;
};

template<class T>
class IFSGenerator : public PlottingGenerator<T> {
	IFS<T> spec_;

public:
	IFSGenerator(int width, int height, const IFS<T>& spec);
	const IFS<T>& specification() const;

protected:
	void exec(int);

	IFS<T>& spec();
};

#endif // IFS_H
