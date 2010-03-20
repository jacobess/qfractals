#include "renderedimage.h"

#include "math.h"

RenderedImage::RenderedImage(int width, int height, int aaDeg, const ColorProvider* colorProvider) :
		img_(width, height),
		aaDeg_(aaDeg),
		types_(new uchar[width * height * aaDeg * aaDeg]),
		vals_(new float[width * height * aaDeg * aaDeg]),
		colorProvider_(colorProvider)
{
	int size = width * height * aaDeg * aaDeg;
	std::fill(types_, types_ + size, 0xff);
}

RenderedImage::~RenderedImage() {
	delete[] types_;
	delete[] vals_;
}

void RenderedImage::setColorProvider(const ColorProvider *colorProvider) {
	colorProvider_ = colorProvider;
}

int RenderedImage::width() const {
	return img_.width();
}

int RenderedImage::height() const {
	return img_.height();
}

int RenderedImage::pointsPerPix() const {
	return aaDeg_ * aaDeg_;
}

const QImage& RenderedImage::image() const {
	return img_.image();
}


void RenderedImage::clear() {
	img_.clear();
	std::fill(types_, types_ + width() * height() * pointsPerPix(), 0xff);
}

void RenderedImage::setSize(int w, int h) {
	setSize(w, h, aaDeg_);
}


void RenderedImage::setSize(int w, int h, int aaDeg) {
	img_.setSize(w, h);

	aaDeg_ = aaDeg;
	int size = w * h * aaDeg * aaDeg;

	delete[] types_;
	delete[] vals_;

	types_ = new uchar[size];
	vals_ = new float[size];

	std::fill(types_, types_ + size, 0xff);
}

void RenderedImage::scale(int cx, int cy, qreal factor) {
	img_.scale(cx, cy, factor);

	uint size = width() * height() * pointsPerPix();
	std::fill(types_, types_ + size, 0xff);
}

void RenderedImage::move(int dx, int dy) {
	img_.move(dx, dy);

	int w = width();
	int h = height();

	uint is = pointsPerPix();

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
}

void RenderedImage::select(qreal wx, qreal wy, qreal hx, qreal hy, qreal x0, qreal y0) {
	img_.select(wx, wy, hx, hy, x0, y0);

	int size = width() * height() * pointsPerPix();
	std::fill(types_, types_ + size, 0xff);
}

void RenderedImage::refreshImage() {
	for(int y = 0; y < height(); y++) {
		for(int x = 0; x < width(); x++) {
			updatePix(x, y);
		}
	}
}


void RenderedImage::updatePix(int x, int y) {
	int cnt = 0;

	float r = 0, g = 0, b = 0, a = 0;

	int ix = pix(x, y, 0);

	const uchar* t = types_ + ix;
	const float* v = vals_ + ix;

	for(int i = 0; i < pointsPerPix(); i++) {
		if((*t) != 0xff) {
			float r0, g0, b0, a0;

			colorProvider_->color(*t, *v, r0, g0, b0, a0);

			r += r0;
			g += g0;
			b += b0;
			a += a0;

			cnt ++;
		}

		t++;
		v++;
	}

	if(cnt > 0) {
		r /= cnt;
		g /= cnt;
		b /= cnt;
		a /= cnt;

		img_.setRgba(x, y, r, g, b, a);
	}
}

int RenderedImage::pix(int x, int y, int index) const {
	return (x + y * width()) * pointsPerPix() + index;
}

bool RenderedImage::isClear(int x, int y, int index) const {
	return types_[pix(x, y, index)] == 0xff;
}

qreal RenderedImage::pointX(int x, int index) const {
	if(aaDeg_ == 1) {
		return x + 0.5;
	} else {
		float inc = 1. / (2 * aaDeg_);

		int ix = (index % aaDeg_);
		int iy = (index / aaDeg_);

		// TODO Check
		return x + qreal(ix) / qreal(aaDeg_) + inc + ((iy % 2) * 2 - 1) * inc / 2.;
	}

}

qreal RenderedImage::pointY(int y, int index) const {
	if(aaDeg_ == 1) {
		return y + 0.5;
	} else {
		float inc = 1. / (2 * aaDeg_);

		int ix = (index % aaDeg_);
		int iy = (index / aaDeg_);

		return y + qreal(iy) / qreal(aaDeg_) + inc + ((ix % 2) * 2 - 1) * inc / 2.;
	}
}

void RenderedImage::setPix(int x, int y, int index, uchar type, float val) {
	uint i = pix(x, y, index);

	types_[i] = type;
	vals_[i] = val;

	if(index == 0) {
		float r, g, b, a;

		colorProvider_->color(type, val,
				     r, g, b, a);

		img_.setRgba(x, y, r, g, b, a);
	} else {
		updatePix(x, y);
	}
}
