#include "specification.h"

#include <QTime>

Generator::Generator(int threadCount, int updateInterval, bool needsImageRefresh, bool isSelectable) :
		isStopped_(false),
		runningCount_(0),
		isSelectable_(isSelectable),
		updateInterval_(updateInterval),
		needsImageRefresh_(needsImageRefresh)
{
	connect(&updateTimer_, SIGNAL(timeout()), this, SLOT(emitUpdate()));

	for(int i = 0; i < threadCount; i++) {
		threads_.push_back(new Thread(i, *this));
	}
}

Generator::~Generator()
{
	// must have been stopped
	qDebug("Destructor Generator");

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

	qDebug("Setting isStopped to false in start()");
	isStopped_ = false;

	startUnlock();
}

void Generator::startUnlock() {
	qDebug("startUnlock");

	// TODO Wait for a short period to avoid threads with 0 runtime

	if(!isStopped_) {
		runningCount_ = threads_.size();

		qDebug("Sending starting signal");
		emit started();

		// Let subclasses do their job
		init();

		qDebug("Starting threads...");

		foreach(Thread* t, threads_) {
			t->start();
		}

		updateTimer_.start(updateInterval_);
	} else {
		isStopped_ = false;
	}

	qDebug("end startUnlock");
	mutex_.unlock();
}

void Generator::cancel() {
	qDebug("Setting isStopped to true");
	isStopped_ = true;
}

void Generator::cancelWait() {
	qDebug("cancelWait");
	lockCancelWait();
	mutex_.unlock();
	qDebug("end cancelWait");
}

void Generator::lockCancelWait() {
	qDebug("lockCancelWait");
	mutex_.lock();

	if(isRunning()) {
		cancel();

		qDebug("Waiting for threads to stop");

		foreach(Thread* t, threads_) {
			t->wait();
		}

		qDebug("Threads stopped");
	}

	qDebug("end lockCancelWait");
}

void Generator::run(int i) {
	qDebug("Running thread %d", i);

	exec(i, threads_.size());

	QMutexLocker locker(&threadMutex_);

	runningCount_ --;

	if(runningCount_ == 0)
	{
		// Stop timer
		updateTimer_.stop();

		if(!isStopped_) {
			emitUpdate();
		}

		qDebug("Sending done signal");

		emit done(isStopped_);

		// So that new cancel-messages can be obtained
		isStopped_ = false;
	}
	qDebug("end Running thread %d", i);
}

bool Generator::isStopped() const {
	return isStopped_;
}

void Generator::emitUpdate() {
	int prgs = progress();
	int total = totalSteps();

	if(needsImageRefresh_) {
		img().refreshImage();
	}

	//qDebug("update %d of %d\n", prgs, total);
	emit updated(prgs, total);
}

Thread::Thread(int index, Generator &parent) :
		index_(index),
		parent_(parent) {}

void Thread::run() {
	qDebug("Launching thread %d", index_);
	QTime time;
	time.start();

	parent_.run(index_);

	qDebug("Thread %d finished after %d ms", index_, time.elapsed());
}

