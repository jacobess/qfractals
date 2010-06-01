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

    virtual void scale(int cx, int cy, double factor) = 0;
    virtual void move(int dx, int dy) = 0;
    virtual void select(double wx, double wy, double hx, double hy, double x0, double y0) = 0;

    virtual void refreshImage() = 0;

    // Transform into normalized coordinates independent of width and height
    double normX(double x) const;
    double normY(double y) const;

    double denormX(double x0) const;
    double denormY(double y0) const;
};

#endif // IMAGE_H
