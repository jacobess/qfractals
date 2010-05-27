#include "rendering.h"
#include "settings.h"

template<class T>
Rendering<T>::Rendering(const Transformation<T>& t) :
		Viewport<T>(t) {}

template<class T>
Rendering<T>::~Rendering() {}

template<class T>
RenderingGenerator<T>::RenderingGenerator(int width, int height, int aa) :
		ViewportGenerator<T>(-1)
{
	renderImg_ = new RenderedImage(width, height, aa, this);
}

template<class T>
RenderingGenerator<T>::~RenderingGenerator() {
	delete renderImg_;
}

template<class T>
int RenderingGenerator<T>::progress() const {
	return pixCount_;
}

template<class T>
int RenderingGenerator<T>::totalSteps() const {
	return renderImg_->width() * renderImg_->height() * renderImg_->indicesPerPixel();
}

template<class T>
void RenderingGenerator<T>::setSize(int width, int height, int aa) {
	this->lockForCommit();

	renderImg_->setSize(width, height, aa);

	this->finishCommit();
}

template<class T>
RenderedImage& RenderingGenerator<T>::img() {
	return *renderImg_;
}

template<class T>
const RenderedImage& RenderingGenerator<T>::img() const {
	return *renderImg_;
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
	RenderingEnv<T>* env = specification().createEnv();

	int w = renderImg_->width();
	int h = renderImg_->height();
	int ppp = renderImg_->indicesPerPixel();

	int tc = this->threadCount();

	int max = w > h ? w : h;

	int idxCount = 1 % tc; // since 0 thread gets the middle

	int x = w / 2;
	int y = h / 2;

	for(int i = 0; i < ppp && !this->isAborted(); i ++) {

		// Middle point is a special case
		if(index == 0) calcPix(x, y, i, 0, env);

		for(int r = 1; r <= (max + 1) / 2 && !this->isAborted(); r ++) {
			for(int a = -r; a < r && !this->isAborted(); a++) {
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
	if(this->isAborted()) return;

	int w = renderImg_->width();
	int h = renderImg_->height();

	if(0 <= x && x < w && 0 <= y && y < h) {

		uchar type;
		double val;

		if(!renderImg_->pix(x, y, i, type, val)) {
			qreal x0 = this->normX(renderImg_->pointX(x, i));
			qreal y0 = this->normY(renderImg_->pointY(y, i));

			T tx = specification().transformation().toX(x0, y0);
			T ty = specification().transformation().toY(x0, y0);

			env->calc(tx, ty, type, val);

			renderImg_->setPix(x, y, i, type, val);
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

template class Rendering<double>;
template class RenderingEnv<double>;
template class RenderingGenerator<double>;

template class Rendering<long double>;
template class RenderingEnv<long double>;
template class RenderingGenerator<long double>;
