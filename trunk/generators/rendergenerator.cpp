#include "rendergenerator.h"


RenderGenerator::RenderGenerator(int width, int height, int aaDeg, const ColorProvider& colorProvider) :
renderImg_(width, height, aaDeg, colorProvider) {}

Image& RenderGenerator::img() {
	return renderImg_;
}

const Image& RenderGenerator::img() const {
	return renderImg_;
}

void RenderGenerator::exec(int index, int count) {
	RenderEnv* env = specification().createEnv();

	int w = renderImg_.width();
	int h = renderImg_.height();
	int ppp = renderImg_.pointsPerPix();

	int max = w > h ? w : h;

	int idxCount = 0;

	int x = w / 2;
	int y = h / 2;

	for(int i = 0; i < ppp && isStopped(); i ++) {
		// Middle point is a special case
		calcPix(x, y, i, env);

		for(int r = 1; r <= (max + 1) / 2 && isStopped(); r ++) {
			for(int a = -r; a < r && isStopped(); a++) {
				if(idxCount == index) {
					// If this thread is responsible
					if(!isStopped()) calcPix(x + a, y - r, i, env);
					if(!isStopped()) calcPix(x - a, y + r, i, env);
					if(!isStopped()) calcPix(x + r, y + a, i, env);
					if(!isStopped()) calcPix(x - r, y - a, i, env);
				}

				idxCount = (idxCount + 1) % count;
			}
		}
	}

	delete(env);
}

void RenderGenerator::init() {
	totalSteps_ = renderImg_.width() * renderImg_.height() * renderImg_.pointsPerPix();
	progress_ = 0;

	renderImg_.init();
}

void RenderGenerator::calcPix(int x, int y, int index, RenderEnv* env) {
	int w = renderImg_.width();
	int h = renderImg_.height();

	if(0 <= x && x < w && 0 <= y && y < h) {
		if(renderImg_.isClear(x, y, index)) {
			qreal x0 = renderImg_.pointX(x, index);
			qreal y0 = renderImg_.pointY(y, index);

			uchar type;
			float val;

			env->calcPix(img().normX(x0), img().normY(y0), type, val);

			renderImg_.setPix(x, y, index, type, val);
		}

		progress_++;
	}
}
