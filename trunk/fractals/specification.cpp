#include "specification.h"

#include <QTime>
#include <QtConcurrentRun>
#include <QVarLengthArray>
#include <QFuture>

Generator::Generator(int threadCount, bool isSelectable) :
		isSelectable_(isSelectable),
		threadCount_(threadCount <= 0 ? QThread::idealThreadCount() : threadCount) {
	threadPool_.setMaxThreadCount(threadCount_);
	notRunningCount_ = 0;
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

void Generator::launch() {
	emitStarted();

	init();

	abort_ = restart_ = dispose_ = false;

	for(int i = 0; i < threadCount_; i++) {
		threadPool_.start(new SubThread(*this, i));
	}
}

void Generator::lockForCommit(bool restart, bool dispose) {
	abortMutex_.lock();
	restart_ = restart;
	dispose_ = dispose;
	abort_ = true;
	rwLock_.lockForWrite(); // Deadlock
	restart_ = false;
	abort_ = false;
	abortMutex_.unlock();
}

void Generator::finishCommit() {
	if(!abort_) {
		emitStarted();
		init();
		waitCondition_.wakeAll();
	}

	rwLock_.unlock();
}

/*void Generator::run() {

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
}*/

void Generator::abort() {
	abortMutex_.lock();
	abort_ = true;
	rwLock_.lockForWrite();
	abort_ = false;
	abortMutex_.unlock();
	rwLock_.unlock();
}

void Generator::resume() {
	abortMutex_.lock();
	abort_ = true;
	rwLock_.lockForWrite();
	abort_ = false;
	abortMutex_.unlock();

	if(!abort_) {
		emitStarted();
		init();
		waitCondition_.wakeAll();
	}

	rwLock_.unlock();
}

bool Generator::isAborted() {
	return abort_;
}

void Generator::dispose() {
	abortMutex_.lock();
	abort_ = true;
	dispose_ = true;
	rwLock_.lockForWrite();
	abort_ = false;
	abortMutex_.unlock();

	waitCondition_.wakeAll();

	rwLock_.unlock();

	threadPool_.waitForDone();
}

void Generator::refresh() {
	if(rwLock_.tryLockForRead()) {
		if(refreshMutex_.tryLock()) {
			img().refreshImage();
			refreshMutex_.unlock();
		}

		rwLock_.unlock();
	}
}

void Generator::emitStopped() {
	executionCountMutex_.lock();
	int i = executionCount_;
	executionCountMutex_.unlock();

	emit executionStopped(i);
}

void Generator::emitStarted() {
	executionCountMutex_.lock();
	int i = ++executionCount_;
	executionCountMutex_.unlock();

	emit executionStarted(i);
}

SubThread::SubThread(Generator &parent, int index) :
		parent_(parent),
		index_(index) {
	setAutoDelete(true);
}

void SubThread::run() {
	parent_.rwLock_.lockForRead();

	while(!parent_.dispose_) {
		QTime time;

		time.start();

		if(!parent_.abort_) parent_.exec(index_);

		// TODO In here count and refresh if last thread
		parent_.notRunningCountMutex_.lock();

		int i = ++parent_.notRunningCount_;

		bool emitDone = i == parent_.threadCount();

		if(emitDone) {
			parent_.notRunningCount_ = 0;

			parent_.notRunningCountMutex_.unlock();

			if(!parent_.restart_ && !parent_.dispose_) {
				parent_.refreshMutex_.lock();
				parent_.img().refreshImage();

				parent_.emitStopped();
			}
		} else {
			parent_.notRunningCountMutex_.unlock();
		}

		qDebug("Thread %d finished as %d th thread after %d ms", index_, i, time.elapsed());

		parent_.waitCondition_.wait(&parent_.rwLock_); // Mark

		parent_.refreshMutex_.unlock();
	}

	parent_.rwLock_.unlock();
}
