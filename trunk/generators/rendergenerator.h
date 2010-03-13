#ifndef RENDERGENERATOR_H
#define RENDERGENERATOR_H

#include "imagegenerator.h"
#include "graphics/renderedimage.h"

class RenderEnv {

public:
	virtual void calcPix(qreal x, qreal y, uchar& type, float& value) = 0;
};

class RenderGenerator;

class RenderSpec {
public:
	virtual RenderEnv* createEnv() const = 0;
};

class RenderGenerator : public SelectableGenerator {

	RenderedImage renderImg_;
	int pixCount_;

public:
	RenderGenerator(int width, int height, int aaDeg,
			const ColorProvider& cp);

	void setAntiAliasingDegree(int aaDeg);

	virtual const RenderSpec& specification() const = 0;

protected:
	Image& img();
	const Image& img() const;

	virtual RenderSpec& specification() = 0;

	void init();

	void exec(int index, int count);

private:
	void calcPix(int x, int y, int i, RenderEnv* env);
};

#endif // RENDERGENERATOR_H
