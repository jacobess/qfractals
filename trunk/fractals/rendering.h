#ifndef RENDERING_H
#define RENDERING_H

#define MAX_LAYERS 255

#include <QVector>

#include "viewport.h"


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

	virtual ImageGenerator* createGenerator(int width, int height) const = 0;
	virtual RenderingEnv<T>* createEnv() const = 0;
};

template<class T>
class RenderingGenerator : public ViewportGenerator<T> {

	// the following are created in the init-method and updated in the
	// calcPixel-method
	int pixCount_;

	QVector<int> count_; // threadcount * 255 elements
	QVector<double> min_;
	QVector<double> max_;
	QVector<double> sum_;
	QVector<double> sqrSum_;

	int aaLevel_;

	uchar* types_;
	double* values_;

public:
	RenderingGenerator(int width, int height, int aa);
	virtual ~RenderingGenerator();

	int progress() const;
	int totalSteps() const;

	void setSize(int width, int height, int aaLevel);

	virtual const Rendering<T>& specification() const = 0;
	virtual void color(uchar type, double value, double& r, double& g, double& b, double& a) const = 0;

	int indicesPerPixel() const;
protected:
	virtual Rendering<T>& spec() = 0;

	void init();

	void exec(int index);

	int count(uchar type) const;
	double min(uchar type) const;
	double max(uchar type) const;
	double sum(uchar type) const;
	double sqrSum(uchar type) const;

	bool isClear(int x, int y, int index) const;
	double pointX(int x, int index) const;
	double pointY(int y, int index) const;

	bool pix(int x, int y, int index, uchar& type, double& value);
	bool pix(int x, int y, int index, double &r, double &g, double &b, double &a);
	void setPix(int x, int y, int index, uchar type, double value);
	void updatePix(int x, int y);

	void setSizeUnsafe(int width, int height);
	void setSizeUnsafe(int width, int height, int aaLevel);
	void scaleUnsafe(int cx, int cy, double factor);
	void moveUnsafe(int dx, int dy);
	void selectUnsafe(double wx, double wy, double hx, double hy, double x0, double y0);

	void refreshUnsafe();

	void clearData();

private:
	void calcPix(int x, int y, int i, int threadIndex, RenderingEnv<T>* env);
};




#endif // RENDERING_H
