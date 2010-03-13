#ifndef IFS_H
#define IFS_H

#include <QList>
#include <QColor>

#include "plotgenerator.h"
#include "rendergenerator.h"
#include "viewportimagegenerator.h"
#include "math/transformation.h"

template<class T>
class IFSEnv;

template<class T>
class IFS : public Viewport<T>, public PlotSpec {

	QList<qreal> probs_;
	qreal totalProbs_;
	QList<Transformation<T> > affTs_;
	QList<QColor> colors_;

public:
    IFS(const Transformation<T>& t,
	const QList<Transformation<T> >& transformations,
	const QList<QColor>& colors);

    IFSEnv<T>* createEnv() const;
};

template<class T>
class IFSEnv : public PlotEnv {
	const IFS<T>& ifs_;

	T x_;
	T y_;

	float r_, g_, b_;

public:
	IFSEnv(const IFS<T>& ifs);
	void nextPix(qreal& x, qreal& y, float& r, float&g, float& b);
};

template<class T>
class IFSGenerator : public ViewportGenerator<T>, public PlotGenerator {
	IFS<T> spec_;

public:
	const IFS<T>& specification() const;

protected:
	IFS<T>& specification();
};

#endif // IFS_H
