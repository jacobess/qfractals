#include "rendering.h"
#include "settings.h"

template<class T>
Rendering<T>::Rendering(const Transformation<T>& t) :
		Viewport<T>(t) {}

template<class T>
Rendering<T>::~Rendering() {}

template<class T>
RenderingGenerator<T>::RenderingGenerator(int width, int height, int aaLevel) :
		ViewportGenerator<T>(-1, width, height),
		aaLevel_(aaLevel),
		types_(new uchar[width * height * aaLevel * aaLevel]),
		values_(new double[width * height * aaLevel * aaLevel]) {
	clearData();
}

template<class T>
RenderingGenerator<T>::~RenderingGenerator() {
	delete[] types_;
	delete[] values_;
}

template<class T>
int RenderingGenerator<T>::progress() const {
	return pixCount_;
}

template<class T>
int RenderingGenerator<T>::totalSteps() const {
	return this->width() * this->height() * indicesPerPixel();
}

template<class T>
void RenderingGenerator<T>::setSizeUnsafe(int width, int height) {
	setSizeUnsafe(width, height, aaLevel_);
}

template<class T>
void RenderingGenerator<T>::setSizeUnsafe(int width, int height, int aaLevel) {
	aaLevel_ = aaLevel;
	int size = width * height * aaLevel * aaLevel;

	delete[] types_;
	delete[] values_;

	types_ = new uchar[size];
	values_ = new double[size];

	ViewportGenerator<T>::setSizeUnsafe(width, height);
	clearData();
	// keep stats as an approximation
}

template<class T>
void RenderingGenerator<T>::scaleUnsafe(int cx, int cy, double factor) {
	ViewportGenerator<T>::scaleUnsafe(cx, cy, factor);
	clearData();
}

template<class T>
void RenderingGenerator<T>::moveUnsafe(int dx, int dy) {
	int w = this->width();
	int h = this->height();

	uint is = indicesPerPixel();

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
					values_[i1 + index] = values_[i0 + index];
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
	ViewportGenerator<T>::moveUnsafe(dx, dy);
}

template<class T>
void RenderingGenerator<T>::selectUnsafe(double wx, double wy, double hx, double hy, double x0, double y0) {
	ViewportGenerator<T>::selectUnsafe(wx, wy, hx, hy, x0, y0);
	clearData();
}

template<class T>
void RenderingGenerator<T>::clearData() {
	std::fill(types_, types_ + this->width() * this->height() * indicesPerPixel(), 0xff);
}

template<class T>
void RenderingGenerator<T>::refreshUnsafe() {
	qDebug("refresh");
	for(int y = 0; y < this->height() && !this->isInterrupted(); y++) {
		for(int x = 0; x < this->width() && !this->isInterrupted(); x++) {
			updatePix(x, y);
		}
	}
}

template<class T>
void RenderingGenerator<T>::init() {
	pixCount_ = 0;

	count_.fill(0, 255 * this->threadCount());
	min_.fill(INFINITY, 255 * this->threadCount());
	max_.fill(-INFINITY, 255 * this->threadCount());
	sum_.fill(0, 255 * this->threadCount());
	sqrSum_.fill(0, 255 * this->threadCount());
}

template<class T>
void RenderingGenerator<T>::exec(int index) {
        // Link to the specification of the generator.
	RenderingEnv<T>* env = spec().createEnv();

	int w = this->width();
	int h = this->height();
	int ppp = indicesPerPixel();

	int tc = this->threadCount();

	int max = w > h ? w : h;

	int idxCount = 1 % tc; // since 0 thread gets the middle

	int x = w / 2;
	int y = h / 2;

	for(int i = 0; i < ppp && !this->isTerminated(); i ++) {

		// Middle point is a special case
		if(index == 0) calcPix(x, y, i, 0, env);

		for(int r = 1; r <= (max + 1) / 2 && !this->isTerminated(); r ++) {
			for(int a = -r; a < r && !this->isTerminated(); a++) {
				if(idxCount == index) {
					// If this thread is responsible
					calcPix(x + a, y - r, i, index, env);
					calcPix(x - a, y + r, i, index, env);
					calcPix(x + r, y + a, i, index, env);
					calcPix(x - r, y - a, i, index, env);
				}

				idxCount++;
				if(idxCount == tc) idxCount = 0;
			}
		}
	}

	delete(env);
}

template<class T>
void RenderingGenerator<T>::calcPix(int x, int y, int i, int threadIndex, RenderingEnv<T>* env) {
	if(this->isTerminated()) return;

	int w = this->width();
	int h = this->height();

	if(0 <= x && x < w && 0 <= y && y < h) {

		uchar type;
		double val;

		if(!pix(x, y, i, type, val)) {
			double x0 = this->normX(pointX(x, i));
			double y0 = this->normY(pointY(y, i));

			T tx = spec().transformation().toX(x0, y0);
			T ty = spec().transformation().toY(x0, y0);

			env->calc(tx, ty, type, val);

			setPix(x, y, i, type, val);
		}

		pixCount_++;

		int idx = threadIndex * 255 + type;

		count_[idx]++;
		if(val < min_[idx]) min_[idx] = val;
		if(val > max_[idx]) max_[idx] = val;
		sum_[idx] += val;
		sqrSum_[idx] += val * val;
	}
}

template<class T>
int RenderingGenerator<T>::count(uchar type) const {
	int total = 0;

	for(int i = type; i < count_.size(); i += 255) {
		total += count_[i];
	}

	return total;
}

template<class T>
double RenderingGenerator<T>::min(uchar type) const {
	double total = -INFINITY;

	for(int i = type; i < min_.size(); i += 255) {
		if(total < min_[i]) total = min_[i];
	}

	return total;
}

template<class T>
double RenderingGenerator<T>::max(uchar type) const {
	double total = -INFINITY;

	for(int i = type; i < max_.size(); i += 255) {
		if(total < max_[i]) total = max_[i];
	}

	return total;
}

template<class T>
double RenderingGenerator<T>::sum(uchar type) const {
	double total = 0;

	for(int i = type; i < sum_.size(); i += 255) {
		total += sum_[i];
	}

	return total;
}

template<class T>
double RenderingGenerator<T>::sqrSum(uchar type) const {
	double total = 0;

	for(int i = type; i < sqrSum_.size(); i += 255) {
		total += sqrSum_[i];
	}

	return total;
}

template<class T>
void RenderingGenerator<T>::updatePix(int x, int y) {
	int incr = (this->width() * y + x) * indicesPerPixel();
	uchar* t = types_ + incr;
	double* v = values_ + incr;

	double r = 0, g = 0, b = 0, a = 0;
	int count = 0;

	for(int i = 0; i < indicesPerPixel(); i++) {
		if(*t != 0xff) {
			count++;

			double r0, g0, b0, a0;
			color(*t, *v, r0, g0, b0, a0);

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

		this->setRgba(x, y, r, g, b, a);
	}
}

template<class T>
bool RenderingGenerator<T>::isClear(int x, int y, int index) const {
	int i = (x + y * this->width()) * indicesPerPixel() + index;

	return types_[i] == 0xff;
}

template<class T>
double RenderingGenerator<T>::pointX(int x, int index) const {
	if(aaLevel_ == 1) {
		return x + 0.5;
	} else {
		double inc = 1. / (2 * aaLevel_);

		int ix = (index % aaLevel_);
		int iy = (index / aaLevel_);

		return x + double(ix) / double(aaLevel_) + inc +
				((iy % 2) * 2 - 1) * inc / 2.;
	}

}

template<class T>
double RenderingGenerator<T>::pointY(int y, int index) const {
	if(aaLevel_ == 1) {
		return y + 0.5;
	} else {
		double inc = 1. / (2 * aaLevel_);

		int ix = (index % aaLevel_);
		int iy = (index / aaLevel_);

		return y + double(iy) / double(aaLevel_) + inc +
				((ix % 2) * 2 - 1) * inc / 2.;
	}
}

template<class T>
bool RenderingGenerator<T>::pix(int x, int y, int index, uchar& type, double& value) {
	int i = (x + y * this->width()) * indicesPerPixel() + index;

	type = types_[i];

	if(type != 0xff) {
		value = values_[i];

		return true;
	} else {
		return false;
	}
}

template<class T>
bool RenderingGenerator<T>::pix(int x, int y, int index, double &r, double &g, double &b, double &a) {

	uchar type;
	double value;

	if(pix(x, y, index, type, value)) {
		color(type, value, r, g, b, a);

		return true;
	} else {
		return false;
	}
}

template<class T>
void RenderingGenerator<T>::setPix(int x, int y, int index, uchar type, double val) {
	int i = (x + y * this->width()) * indicesPerPixel() + index;

	types_[i] = type;
	values_[i] = val;

	if(index == 0) {
		double r, g, b, a;

		pix(x, y, 0, r, g, b, a);

		this->setRgba(x, y, r, g, b, a);
	} else {
		updatePix(x, y);
	}
}

template<class T>
int RenderingGenerator<T>::indicesPerPixel() const {
	return aaLevel_ * aaLevel_;
}

template class Rendering<double>;
template class RenderingEnv<double>;
template class RenderingGenerator<double>;

template class Rendering<long double>;
template class RenderingEnv<long double>;
template class RenderingGenerator<long double>;
