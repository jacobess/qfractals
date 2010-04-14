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
	lockCancelWait();

	img().setSize(width, height);

	startUnlock();
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

void Generator::start() {
	mutex_.lock();

	//qDebug("Setting isStopped to false in start()");
	isStopped_ = false;

	startUnlock();
}

void Generator::startUnlock() {
	//qDebug("startUnlock");

	// TODO Wait for a short period to avoid threads with 0 runtime

	if(!isStopped_) {
		runningCount_ = threads_.size();

		//qDebug("Sending starting signal");
		emit started();

		//qDebug("Starting thread 0...");
		threads_[0]->start();
	} else {
		isStopped_ = false;
	}

	//qDebug("end startUnlock");
	mutex_.unlock();
}

void Generator::cancel() {
	//qDebug("Setting isStopped to true");
	isStopped_ = true;
}

void Generator::cancelWait() {
	//qDebug("cancelWait");
	lockCancelWait();
	mutex_.unlock();
	//qDebug("end cancelWait");
}

void Generator::lockCancelWait() {
	////qDebug("lockCancelWait");
	mutex_.lock();

	if(isRunning()) {
		cancel();

		//qDebug("Waiting for threads to stop");

		foreach(Thread* t, threads_) {
			t->wait();
		}

		//qDebug("Threads stopped");
	}

	////qDebug("end lockCancelWait");
}

void Generator::run(int i) {
	if(i == 0) {
		//qDebug("Zero-Thread starting other threads");
		// Zero-Thread initializes image and starts other threads
		init();

		for(int i = 1; i < threads_.size(); i++) {
			threads_[i]->start();
		}
	}

	if(!isStopped()) {
		exec(i, threads_.size());
	} else {
		//qDebug("Do not execute thread %d since already stopped", i);
	}

	QMutexLocker locker(&threadMutex_);
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

