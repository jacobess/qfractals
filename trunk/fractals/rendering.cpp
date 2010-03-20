#include "rendering.h"
#include "settings.h"

template<class T>
Rendering<T>::Rendering(const Transformation<T>& t) :
		Viewport<T>(t) {}

template<class T>
Rendering<T>::~Rendering() {}

template<class T>
RenderingGenerator<T>::RenderingGenerator(int width, int height, int aa, const ColorProvider* cp) :
		ViewportGenerator<T>(Settings::settings()->threadCount(),
				     Settings::settings()->updateInterval(),
				     false),
		renderImg_(width, height, aa, cp) {}

template<class T>
RenderingGenerator<T>::~RenderingGenerator() {}

template<class T>
int RenderingGenerator<T>::progress() {
	return pixCount_;
}

template<class T>
int RenderingGenerator<T>::totalSteps() {
	return renderImg_.width() * renderImg_.height() * renderImg_.pointsPerPix();
}

template<class T>
void RenderingGenerator<T>::setSize(int width, int height, int aa) {
	this->lockCancelWait();
	renderImg_.setSize(width, height, aa);
	this->startUnlock();
}

template<class T>
RenderedImage& RenderingGenerator<T>::img() {
	return renderImg_;
}

template<class T>
const RenderedImage& RenderingGenerator<T>::img() const {
	return renderImg_;
}

template<class T>
void RenderingGenerator<T>::init() {
	pixCount_ = 0;
}

template<class T>
void RenderingGenerator<T>::exec(int index, int count) {
	RenderingEnv<T>* env = specification().createEnv();

	int w = renderImg_.width();
	int h = renderImg_.height();
	int ppp = renderImg_.pointsPerPix();

	int max = w > h ? w : h;

	int idxCount = 1; // since 0 thread gets the middle

	int x = w / 2;
	int y = h / 2;

	for(int i = 0; i < ppp && !this->isStopped(); i ++) {

		// Middle point is a special case
		if(index == 0) calcPix(x, y, i, env);

		for(int r = 1; r <= (max + 1) / 2 && !this->isStopped(); r ++) {
			for(int a = -r; a < r && !this->isStopped(); a++) {
				if(idxCount == index) {
					// If this thread is responsible
					if(!this->isStopped()) calcPix(x + a, y - r, i, env);
					if(!this->isStopped()) calcPix(x - a, y + r, i, env);
					if(!this->isStopped()) calcPix(x + r, y + a, i, env);
					if(!this->isStopped()) calcPix(x - r, y - a, i, env);
				}

				idxCount = (idxCount + 1) % count;
			}
		}
	}

	delete(env);
}

template<class T>
void RenderingGenerator<T>::calcPix(int x, int y, int i, RenderingEnv<T>* env) {
	int w = renderImg_.width();
	int h = renderImg_.height();

	if(0 <= x && x < w && 0 <= y && y < h) {
		if(renderImg_.isClear(x, y, i)) {
			qreal x0 = this->normX(renderImg_.pointX(x, i));
			qreal y0 = this->normY(renderImg_.pointY(y, i));

			T tx = specification().transformation().toX(x0, y0);
			T ty = specification().transformation().toY(x0, y0);

			uchar type;
			float val;

			env->calc(tx, ty, type, val);

			renderImg_.setPix(x, y, i, type, val);
		}

		pixCount_++;
	}
}

template class Rendering<qreal>;
template class RenderingEnv<qreal>;
template class RenderingGenerator<qreal>;

template class Rendering<long double>;
template class RenderingEnv<long double>;
template class RenderingGenerator<long double>;
