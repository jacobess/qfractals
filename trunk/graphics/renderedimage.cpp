#include "renderedimage.h"

#include "math.h"

#include <QTime>

RenderedImage::RenderedImage(int width, int height, int aaDeg, const ColorProvider* colorProvider) :
		width_(width),
		height_(height),
		aaDeg_(aaDeg),
		img_(width, height),
		types_(new uchar[width * height * aaDeg * aaDeg]),
		vals_(new double[width * height * aaDeg * aaDeg]),
		colorProvider_(colorProvider)
{
	clear();
}

RenderedImage::~RenderedImage() {
	delete[] types_;
	delete[] vals_;
}

int RenderedImage::width() const {
	return width_;
}

int RenderedImage::height() const {
	return height_;
}

int RenderedImage::indicesPerPixel() const {
	return aaDeg_ * aaDeg_;

}

const QImage& RenderedImage::image() const {
	return img_.image();
}

void RenderedImage::setSize(int w, int h) {
	setSize(w, h, aaDeg_);
}

void RenderedImage::setSize(int w, int h, int aaDeg) {
	aaDeg_ = aaDeg;
	int size = w * h * aaDeg * aaDeg;

	delete[] types_;
	delete[] vals_;

	types_ = new uchar[size];
	vals_ = new double[size];

	width_ = w;
	height_ = h;

	img_.setSize(w, h);
	clearData();
	// keep the stats as an approximation
}

void RenderedImage::clear() {
	img_.clear();
	clearData();
}

void RenderedImage::clearData() {
	std::fill(types_, types_ + width_ * height_ * aaDeg_ * aaDeg_, 0xff);
}

void RenderedImage::scale(int cx, int cy, double factor) {
	img_.scale(cx, cy, factor);
	clearData();
}

void RenderedImage::move(int dx, int dy) {
	int w = width_;
	int h = height_;

	uint is = aaDeg_ * aaDeg_;

	// TODO: More elegant
	int x0, x1, ix;
	int y0, y1, iy;

	if(dx >= 0) {
		x0 = w - 1;
		x1 = 0;
		ix = -1;
	} else {
		x0 = 0;
		x1 = w - 1;
		ix = 1;
	}

	if(dy >= 0) {
		y0 = h - 1;
		y1 = 0;
		iy = -1;
	} else {
		y0 = 0;
		y1 = h - 1;
		iy = 1;
	}

	for(int x = x0;; x += ix) {
		for(int y = y0;; y += iy) {
			int i1 = (x + w * y) * is;

			if(dx <= x && x < w + dx &&
			   dy <= y && y < h + dy) {
				uint i0 = ((x - dx) + w * (y - dy)) * is;

				for(uint index = 0; index < is; index++) {
					types_[i1 + index] = types_[i0 + index];
					vals_[i1 + index] = vals_[i0 + index];
				}
			} else {
				for(uint index = 0; index < is; index++) {
					types_[i1 + index] = 0xff;
				}
			}

			if(y == y1) break;
		}

		if(x == x1) break;
	}

	// data is still valid
	img_.move(dx, dy);
}

void RenderedImage::select(double wx, double wy, double hx, double hy, double x0, double y0) {
	img_.select(wx, wy, hx, hy, x0, y0);
	clearData();
}

void RenderedImage::refreshImage() {
	// TODO This is very slow and takes more than 1 second
	for(int y = 0; y < height_; y++) {
		for(int x = 0; x < width_; x++) {
			updatePix(x, y);
		}
	}
}

void RenderedImage::updatePix(int x, int y) {
	int incr = (width_ * y + x) * indicesPerPixel();
	uchar* t = types_ + incr;
	double* v = vals_ + incr;

	double r = 0, g = 0, b = 0, a = 0;
	int count = 0;

	for(int i = 0; i < indicesPerPixel(); i++) {
		if(*t != 0xff) {
			count++;

			double r0, g0, b0, a0;
			colorProvider_->color(*t, *v, r0, g0, b0, a0);

			r += r0;
			g += g0;
			b += b0;
			a += a0;
		}

		t++;
		v++;
	}

	if(count > 0) {
		r /= count;
		g /= count;
		b /= count;
		a /= count;

		img_.setRgba(x, y, r, g, b, a);
	}
}

bool RenderedImage::isClear(int x, int y, int index) const {
	int i = (x + y * width_) * aaDeg_ * aaDeg_ + index;

	return types_[i] == 0xff;
}

double RenderedImage::pointX(int x, int index) const {
	if(aaDeg_ == 1) {
		return x + 0.5;
	} else {
		double inc = 1. / (2 * aaDeg_);

		int ix = (index % aaDeg_);
		int iy = (index / aaDeg_);

		return x + double(ix) / double(aaDeg_) + inc +
				((iy % 2) * 2 - 1) * inc / 2.;
	}

}

double RenderedImage::pointY(int y, int index) const {
	if(aaDeg_ == 1) {
		return y + 0.5;
	} else {
		double inc = 1. / (2 * aaDeg_);

		int ix = (index % aaDeg_);
		int iy = (index / aaDeg_);

		return y + double(iy) / double(aaDeg_) + inc +
				((ix % 2) * 2 - 1) * inc / 2.;
	}
}

bool RenderedImage::pix(int x, int y, int index, uchar& type, double& value) {
	int i = (x + y * width_) * aaDeg_ * aaDeg_ + index;

	type = types_[i];

	if(type != 0xff) {
		value = vals_[i];

		return true;
	} else {
		return false;
	}
}

bool RenderedImage::pix(int x, int y, int index, double &r, double &g, double &b, double &a) {

	uchar type;
	double value;

	if(pix(x, y, index, type, value)) {
		colorProvider_->color(type, value, r, g, b, a);

		return true;
	} else {
		return false;
	}
}

void RenderedImage::setPix(int x, int y, int index, uchar type, double val) {
	int i = (x + y * width_) * aaDeg_ * aaDeg_ + index;

	types_[i] = type;
	vals_[i] = val;

	if(index == 0) {
		double r, g, b, a;

		pix(x, y, 0, r, g, b, a);

		img_.setRgba(x, y, r, g, b, a);
	} else {
		updatePix(x, y);
	}
}
