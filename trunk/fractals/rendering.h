#ifndef RENDERING_H
#define RENDERING_H

#include <QVector>

#include "viewport.h"
#include "graphics/renderedimage.h"

template<class T>
class RenderingEnv {

public:
	virtual void calc(const T& x, const T& y, uchar& type, double& value) = 0;
};


template<class T>
class Rendering : public Viewport<T> {
public:
	Rendering(const Transformation<T>& t);
	virtual ~Rendering();

	virtual Generator* createGenerator(int width, int height) const = 0;
	virtual RenderingEnv<T>* createEnv() const = 0;
};

template<class T>
class RenderingGenerator : public ViewportGenerator<T>, public ColorProvider {

	RenderedImage* renderImg_;

	// The following are set in the init-method
	int threadCount_;

	// the following are created in the init-method and updated in the
	// calcPixel-method
	int pixCount_;

	QVector<int> count_; // threadcount * 255 elements
	QVector<double> min_;
	QVector<double> max_;
	QVector<double> sum_;
	QVector<double> sqrSum_;

public:
	RenderingGenerator(int width, int height, int aa);
	virtual ~RenderingGenerator();

	int progress() const;
	int totalSteps() const;

	void setSize(int width, int height, int aa);

	virtual const Rendering<T>& specification() const = 0;
	virtual void color(uchar type, double value, double& r, double& g, double& b, double& a) const = 0;

protected:
	virtual Rendering<T>& specification() = 0;

	RenderedImage& img();
	const RenderedImage& img() const;

	void init();

	void exec(int index);

	int count(uchar type) const;
	double min(uchar type) const;
	double max(uchar type) const;
	double sum(uchar type) const;
	double sqrSum(uchar type) const;

private:
	void calcPix(int x, int y, int i, int threadIndex, RenderingEnv<T>* env);
};




#endif // RENDERING_H
