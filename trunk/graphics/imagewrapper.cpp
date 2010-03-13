#include "imagewrapper.h"

#include <QPainter>

ImageWrapper::ImageWrapper(int width, int height) :
		img_(width, height, QImage::Format_ARGB32) {
	data_ = (uint*) img_.bits();
}

/*ImageWrapper::ImageWrapper(const ImageWrapper &image) : Image() {
	this->image = new QImage(image.width(), image.height(), QImage::Format_ARGB32);
	data_ = (int*) this->image->bits();

	std::copy(image.data_, image.data_ + image.width() * image.height(), data_);
}*/

ImageWrapper::~ImageWrapper() {}

int ImageWrapper::width() const {
	return img_.width();
}

int ImageWrapper::height() const {
	return img_.height();
}

const QImage& ImageWrapper::image() const {
	return img_;
}

QImage& ImageWrapper::image() {
	return img_;
}

void ImageWrapper::refreshImage() {
	// Do nothing
}

void ImageWrapper::clear() {
	std::fill(data_, data_ + width() * height(), 0x0);
}

void ImageWrapper::setSize(int w, int h) {
	qreal x0 = normX(0);
	qreal y0 = normY(0);

	qreal x1 = normX(width());
	qreal y1 = normY(height());

	QImage oldImage = img_;
	img_ = QImage(w, h, QImage::Format_ARGB32);
	data_ = (uint*) img_.bits();
	QPainter painter(&img_);

	painter.fillRect(img_.rect(), Qt::black);

	painter.setRenderHint(QPainter::SmoothPixmapTransform);

	x0 = denormX(x0);
	y0 = denormY(y0);

	x1 = denormX(x1);
	y1 = denormY(y1);

	QRect newRect(x0, y0, x1 - x0, y1 - y0);

	painter.drawImage(newRect, oldImage);
}

void ImageWrapper::scale(int cx, int cy, qreal factor) {
	QImage newImage(width(), height(), QImage::Format_ARGB32);

	QPainter painter(&newImage);

	painter.fillRect(newImage.rect(), Qt::black);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);

	painter.translate(cx, cy);
	painter.scale(1 / factor, 1 / factor);
	painter.translate(-cx, -cy);

	painter.drawImage(0, 0, img_);

	img_ = newImage;
	data_ = (uint*) img_.bits();
}

void ImageWrapper::move(int dx, int dy) {
	QImage newImage(width(), height(), QImage::Format_ARGB32);

	QPainter painter(&newImage);
	painter.fillRect(newImage.rect(), Qt::black);

	painter.translate(dx, dy);

	painter.drawImage(0, 0, img_);

	img_ = newImage;
	data_ = (uint*) img_.bits();
}

void ImageWrapper::select(qreal wx, qreal wy, qreal hx, qreal hy, qreal x0, qreal y0) {
	QTransform t(wx, wy, hx, hy, x0, y0);

	QImage newImage(width(), height(), QImage::Format_ARGB32);

	QPainter painter(&newImage);
	painter.fillRect(newImage.rect(), Qt::black);

	painter.setTransform(t.inverted());
	painter.drawImage(0, 0, img_);

	img_ = newImage;
	data_ = (uint*) img_.bits();
}

void ImageWrapper::rgba(int x, int y, float& r, float& g, float& b, float& a) const {
	uint argb = data_[x + y * width()];

	r = ((argb >> 16) & 0xff) / 255.f;
	g = ((argb >> 8) & 0xff) / 255.f;
	b = (argb & 0xff) / 255.f;
	a = ((argb >> 24) & 0xff) / 255.f;
}

void ImageWrapper::setRgba(int x, int y, float r, float g, float b, float a) {
	// Round
	r = r * 255 + 0.5;
	g = g * 255 + 0.5;
	b = b * 255 + 0.5;
	a = a * 255 + 0.5;

	uint argb = int(a) << 24 | int(r) << 16 | int(g) << 8 | int(b);
	data_[x + y * width()] = argb;
}
