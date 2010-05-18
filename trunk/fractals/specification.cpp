#include "specification.h"

#include <QTime>

Generator::Generator(int threadCount, bool isSelectable) :
		isStopped_(false),
		runningCount_(0),
		isSelectable_(isSelectable)
{
	for(int i = 0; i < threadCount; i++) {
		threads_.push_back(new Thread(i, *this));
	}
}

Generator::~Generator()
{
	// must have been stopped
	//qDebug("Destructor Generator");

	foreach(Thread* t, threads_) {
		delete t;
	}
}

const QImage& Generator::image() const {
	return img().image();
}

void Generator::setSize(int width, int height) {
	QMutexLocker locker(&threadMutex_);

	cancelWaitUnsafe();
	img().setSize(width, height);
	startUnsafe();
}

int Generator::width() const {
	return img().width();
}

int Generator::height() const {
	return img().height();
}

qreal Generator::normX(qreal x) const {
	return img().normX(x);
}

qreal Generator::normY(qreal y) const {
	return img().normY(y);
}

qreal Generator::denormX(qreal x0) const {
	return img().denormX(x0);
}

qreal Generator::denormY(qreal y0) const {
	return img().denormY(y0);
}


bool Generator::isRunning() const {
	return runningCount_ > 0;
}

int Generator::threadCount() const {
	return threads_.size();
}


void Generator::start() {
	QMutexLocker locker(&threadMutex_);
	isStopped_ = false;
	startUnsafe();
}

void Generator::startUnsafe() {
	if(!isStopped_) {
		runningCount_ = threads_.size();

		emit started();

		init();

		for(int i = 0; i < threads_.size(); i++) {
			threads_[i]->start();
		}
	} else {
		isStopped_ = false;
	}
}

void Generator::cancel() {
	//qDebug("Setting isStopped to true");
	isStopped_ = true;
}

void Generator::cancelWait() {
	QMutexLocker locker(&threadMutex_);
	cancelWaitUnsafe();
}

void Generator::cancelWaitUnsafe() {
	if(isRunning()) {
		cancel();

		foreach(Thread* t, threads_) {
			t->wait();
		}
	}
}

void Generator::run(int i) {
	if(!isStopped()) {
		exec(i);
	} else {
		//qDebug("Do not execute thread %d since already stopped", i);
	}

	// TODO: Dead lock with other thread
	QMutexLocker locker(&mutex_);
	runningCount_ --;

	if(runningCount_ == 0)
	{
		//qDebug("Last thread running, stopping");

		emit done(isStopped_);

		// So that new cancel-messages can be obtained
		isStopped_ = false;
	}
}

bool Generator::isStopped() const {
	return isStopped_;
}

void Generator::refresh() {
	img().refreshImage();
}

Thread::Thread(int index, Generator &parent) :
		index_(index),
		parent_(parent) {}

void Thread::run() {
	//qDebug("Launching thread %d", index_);
	QTime time;
	time.start();

	parent_.run(index_);

	qDebug("Thread %d finished after %d ms", index_, time.elapsed());
}

