#include "specification.h"

#include <QTime>
#include <QtConcurrentRun>
#include <QVarLengthArray>
#include <QFuture>

Generator::Generator(int threadCount, bool isSelectable) :
		isSelectable_(isSelectable),
		threadCount_(threadCount <= 0 ? QThread::idealThreadCount() : threadCount) {
	threadPool.setMaxThreadCount(threadCount_);
}

Generator::~Generator() {
}

const QImage& Generator::image() const {
	return img().image();
}

void Generator::setSize(int width, int height) {
	lockForCommit();
	img().setSize(width, height);
	finishCommit();
}

void Generator::lockForCommit() {
	restartMutex_.lock();
	restart_ = true;
	abort_ = true;
	runningMutex_.lock();
	restart_ = false;
	abort_ = false;
	restartMutex_.unlock();
	refreshMutex_.lock();
}

void Generator::finishCommit() {
	restartMutex_.lock();

	if(!restart_) {
		waitCondition_.wakeOne();
	}

	runningMutex_.unlock();
	restartMutex_.unlock();
	refreshMutex_.unlock();
}


int Generator::width() const {
	return img().width();
}

int Generator::height() const {
	return img().height();
}

double Generator::normX(double x) const {
	return img().normX(x);
}

double Generator::normY(double y) const {
	return img().normY(y);
}

double Generator::denormX(double x0) const {
	return img().denormX(x0);
}

double Generator::denormY(double y0) const {
	return img().denormY(y0);
}

int Generator::threadCount() const {
	return threadCount_;
}

void Generator::run() {

	QVarLengthArray< SubThread* > subthreads;
	subthreads.resize(threadCount_);

	for(int i = 0; i < threadCount_; i++) {
		subthreads[i] = new SubThread(*this, i);
	}

	restart_ = false;
	abort_ = false;
	dispose_ = false;
	runningMutex_.lock();

	int executionIndex = 0;

	forever {
		emit executionStarted(executionIndex);

		init();

		QTime time;
		time.start();

		for(int i = 0; i < threadCount_; i++) {
			threadPool.start(subthreads[i]);
		}

		threadPool.waitForDone();

		qDebug("Threads finished after %d ms", time.elapsed());

		if(!restart_) {
			refreshMutex_.lock();
			img().refreshImage();
			refreshMutex_.unlock();

			emit executionStopped(executionIndex++);
		}

		waitCondition_.wait(&runningMutex_);

		if(dispose_) {
			break;
		}
	}

	for(int i = 0; i < threadCount_; i++) {
		delete subthreads[i];
	}
}

void Generator::abort() {
	restartMutex_.lock();
	abort_ = true;
	runningMutex_.lock();
	abort_ = false;
	restartMutex_.unlock();
	runningMutex_.unlock();
}

void Generator::resume() {
	if(runningMutex_.tryLock()) {
		waitCondition_.wakeOne();
		runningMutex_.unlock();
	}
}

bool Generator::isAborted() {
	return abort_;
}

void Generator::dispose() {
	restartMutex_.lock();
	abort_ = true;
	runningMutex_.lock();
	abort_ = false;
	dispose_ = true;
	restartMutex_.unlock();
	waitCondition_.wakeOne();
	runningMutex_.unlock();

	refreshMutex_.lock();
	refreshMutex_.unlock();
}

void Generator::refresh() {
	refreshMutex_.lock();
	img().refreshImage();
	refreshMutex_.unlock();
}

SubThread::SubThread(Generator &parent, int index) :
		parent_(parent),
		index_(index) {
	setAutoDelete(false);
}

void SubThread::run() {
	parent_.exec(index_);
}
