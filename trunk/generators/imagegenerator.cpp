#include <math.h>
#include "imagegenerator.h"

void SelectableGenerator::setSize(int width, int height) {
	QMutexLocker locker(&mutex);

	Generator::cancelAndWaitUnsafe();
	img().setSize(width, height);
	emit resized(width, height);
	startUnsafe();
}

int SelectableGenerator::width() const {
	return img().width();
}

int SelectableGenerator::height() const {
	return img().height();
}

const QImage& SelectableGenerator::image() {
	return img().image();
}

const QImage& SelectableGenerator::image() const {
	return img().image();
}
