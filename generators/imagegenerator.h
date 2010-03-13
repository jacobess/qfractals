#ifndef IMAGEGENERATOR_H
#define IMAGEGENERATOR_H

#include <QImage>

#include "generator.h"
#include "graphics/image.h"
#include "math/transformation.h"

class SelectableGenerator : public Generator {
	Q_OBJECT

public:
	virtual ~SelectableGenerator() {};

	int width() const;
	int height() const;

	const QImage& image();
	const QImage& image() const;

	void setSize(int width, int height);

	virtual void scale(int cx, int cy, qreal factor) = 0;
	virtual void move(int dx, int dy) = 0;
	virtual void select(qreal wx, qreal wy, qreal hx, qreal hy, qreal x0, qreal y0) = 0;

signals:
	void resized(int width, int height);

protected:
	virtual Image& img() = 0;
	virtual const Image& img() const = 0;
};

#endif // IMAGEGENERATOR_H
