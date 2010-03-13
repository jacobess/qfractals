#ifndef IMAGE_H
#define IMAGE_H

#include<sys/types.h>
#include<qtransform>
#include<qimage>

class Image {
public:
    virtual int width() const = 0;
    virtual int height() const = 0;

    virtual const QImage& image() const = 0;

    virtual void clear() = 0;

    virtual void setSize(int width, int height) = 0;

    virtual void scale(int cx, int cy, qreal factor) = 0;
    virtual void move(int dx, int dy) = 0;
    virtual void select(qreal wx, qreal wy, qreal hx, qreal hy, qreal x0, qreal y0) = 0;

    virtual void refreshImage() = 0;

    // Transform into normalized coordinates independent of width and height
    qreal normX(qreal x) const;
    qreal normY(qreal y) const;

    qreal denormX(qreal x0) const;
    qreal denormY(qreal y0) const;
};

#endif // IMAGE_H
