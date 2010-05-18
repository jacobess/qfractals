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

	refreshThread_ = new RefreshThread(*this);
}

Generator::~Generator()
{
	// must have been stopped
	//qDebug("Destructor Generator");

	foreach(Thread* t, threads_) {
		delete t;
	}

	delete refreshThread_;
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
		//qDebug("Start Unsafe");

		//qDebug("SU: Running Count Set");

		emit started();

		qDebug("SU: Started-signal emitted");

		init();

		//qDebug("SU: Initialization done");

		runningCountMutex_.lock();

		for(int i = 0; i < threads_.size(); i++) {
			threads_[i]->start();
		}

		runningCount_ = threads_.size();

		runningCountMutex_.unlock();

		//qDebug("SU: Threads started");
	} else {
		//qDebug("SU: Not starting since already stopped");
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
	}

	foreach(Thread* t, threads_) {
		t->wait();
	}

	refreshThread_->wait();
}

bool Generator::isStopped() const {
	return isStopped_;
}

void Generator::refresh() {
	if(!refreshThread_->isRunning()) {
		refreshThread_->start(QThread::LowPriority);
	}
}

void Generator::emitDoneSignal() {
	emit done(isStopped_);
}

Thread::Thread(int index, Generator &parent) :
		index_(index),
		parent_(parent) {}

void Thread::run() {
	//qDebug("Launching thread %d", index_);
	QTime time;
	time.start();

	parent_.exec(index_);

	QMutexLocker locker(&parent_.runningCountMutex_);

	if(parent_.runningCount_ == 1)
	{
		qDebug("%d: Emitting done-signal", index_);

		if(!parent_.isStopped_) {
			parent_.refresh();
			parent_.refreshThread_->wait();
		}

		parent_.emitDoneSignal();

		qDebug("%d: Done-signal emitted", index_);

		// So that new cancel-messages can be obtained
		parent_.isStopped_ = false;
	}

	parent_.runningCount_ --;

	qDebug("Thread %d finished after %d ms", index_, time.elapsed());
}

RefreshThread::RefreshThread(Generator &parent) :
		parent_(parent) {}

void RefreshThread::run() {
	parent_.img().refreshImage();
}
