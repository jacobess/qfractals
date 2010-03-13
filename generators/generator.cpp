#include "generator.h"
#include "settings.h"

Generator::Generator() :
		isStopped_(false),
		runningCount_(0),
		progress_(0),
		totalSteps_(0)
{
	connect(&timer_, SIGNAL(timeout()), this, SLOT(emitUpdated()));

	int threadCount = Settings::settings()->threadCount();

	for(int i = 0; i < threadCount; i++) {
		threads_.push_back(new Thread(i, *this));
	}
}

Generator::~Generator()
{
	QMutexLocker locker(&mutex);

	qDebug("Destructor Generator");

	cancelAndWaitUnsafe();

	foreach(Thread* t, threads_) {
		delete t;
	}
}

bool Generator::isRunning() const {
	return runningCount_ > 0;
}

void Generator::start() {
	QMutexLocker locker(&mutex);

	startUnsafe();
}

void Generator::startUnsafe() {
	cancelAndWaitUnsafe();

	isStopped_ = false;
	progress_ = 0;

	runningCount_ = threads_.size();

	// Let subclasses do their job
	init();

	qDebug("Starting threads...");

	foreach(Thread* t, threads_) {
		t->start();
	}

	int updateInterval = Settings::settings()->updateInterval();
	timer_.start(updateInterval);

	emit started();
}

void Generator::cancel() {
	qDebug("Cancelling running threads");
	isStopped_ = true;
}

void Generator::cancelAndWaitUnsafe() {
	if(runningCount_ > 0) {
		cancel();

		qDebug("Waiting for threads to stop");

		foreach(Thread* t, threads_) {
			t->wait();
		}

		qDebug("Threads stopped");
	}
}

void Generator::run(int i) {
	exec(i, threads_.size());

	QMutexLocker locker(&threadMutex);

	qDebug("Thread %d has stopped", i);

	runningCount_ --;

	if(runningCount_ == 0)
	{
		progress_ = totalSteps_;
		timer_.stop();
		emitUpdate();
		emit done(isStopped_);

		qDebug("All threads have stopped");
	}
}

bool Generator::isStopped() const {
	return isStopped_;
}

void Generator::setProgress(int progress, int totalSteps) {
	progress_ = progress;
	totalSteps_ = totalSteps;
}

void Generator::emitUpdate() {
	emit updated(progress_, totalSteps_);
}

Thread::Thread(int index, Generator &parent) :
		index_(index),
		parent_(parent) {}

void Thread::run() {
	qDebug("Launching thread %d", index_);
	parent_.run(index_);
}
