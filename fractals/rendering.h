#ifndef RENDERING_H
#define RENDERING_H

#include "viewport.h"
#include "graphics/renderedimage.h"

template<class T>
class RenderingEnv {

public:
	virtual void calc(const T& x, const T& y, uchar& type, float& value) = 0;
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
class RenderingGenerator : public ViewportGenerator<T> {

	RenderedImage renderImg_;
	int pixCount_;

public:
	RenderingGenerator(int width, int height, int aa, const ColorProvider* cp);
	virtual ~RenderingGenerator();

	int progress();
	int totalSteps();

	void setSize(int width, int height, int aa);

	virtual const Rendering<T>& specification() const = 0;

protected:
	RenderedImage& img();
	const RenderedImage& img() const;

	virtual Rendering<T>& specification() = 0;

	void init();

	void exec(int index, int count);

private:
	void calcPix(int x, int y, int i, RenderingEnv<T>* env);
};

#endif // RENDERING_H
