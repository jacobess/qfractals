#include "specification.h"

#include <QTime>
#include <QtConcurrentRun>
#include <QVarLengthArray>
#include <QFuture>

#include <QPainter>

WorkingThread::WorkingThread(Generator &parent) :
		parent_(parent) {}

WorkingThread::~WorkingThread() {}


void WorkingThread::run() {
	parent_.workerLock_.lockForRead();

	parent_.waitingCountMutex_.lock();
	int index = parent_.waitingCount_ ++;
	parent_.waitingCountMutex_.unlock();

	while(!parent_.dispose_) {

		qDebug("Thread %d waiting", index);

		// Wait until woken up by main thread
		parent_.workerCondition_.wait(&parent_.workerLock_);

		qDebug("Thread %d woke up", index);

		parent_.runningCountMutex_.lock();
		parent_.runningCount_ ++;
		parent_.runningCountMutex_.unlock();

		qDebug("Thread %d started", index);

		parent_.exec(index);

		// Wait until all threads have started

		qDebug("Thread %d finished", index);
	}

	parent_.workerLock_.unlock();
}

Generator::Generator(int threadCount) :
		threadCount_(threadCount <= 0 ? QThread::idealThreadCount() : threadCount),
		running_(false),
		terminate_(false),
		interrupt_(false),
		dispose_(false),
		waitingCount_(0),
		runningCount_(0) {}

Generator::~Generator() {
	qDebug("Destructor");
}

bool Generator::isRunning() const {
	return running_;
}

void Generator::terminate() {
	flagMutex_.lock();

	if(running_) {
		terminate_ = true;
	}

	flagMutex_.unlock();
}

void Generator::interrupt() {
	flagMutex_.lock();

	if(running_) {
		interrupt_ = true;
	}

	flagMutex_.unlock();
}

void Generator::dispose() {
	dispose_ = true;

	// wait until main thread is in waiting position
	allowChangesMutex_.lock();
	allowChangesMutex_.unlock();

	allowChangesWaitCondition_.wakeAll();

	this->wait();
}

void Generator::run() {
	// Create and start working threads

	WorkingThread** threads = new WorkingThread*[threadCount_];

	for(int i = 0; i < threadCount_; i++) {
		threads[i] = new WorkingThread(*this);
	}

	for(int i = 0; i < threadCount_; i++) {
		threads[i]->start(QThread::LowPriority);
	}

	// Wait until all threads are in waiting position
	waitingCountMutex_.lock();

	while(waitingCount_ < threadCount_) {
		waitingCountMutex_.unlock();
		QThread::msleep(10);
		waitingCountMutex_.lock();
	}

	waitingCountMutex_.unlock();

	qDebug("Trying to obtain writeLock");

	workerLock_.lockForWrite();

	qDebug("Obtained writeLock");

	allowChangesMutex_.lock();

	int turnCount = 0;

	// Main loop
	while(!dispose_) {
		qDebug("Starting...");
		// Running part start
		flagMutex_.lock();
		running_ = true;
		emit started(++turnCount);
		flagMutex_.unlock();

		// Initialize image
		QTime time;
		time.start(); // For timing purposes

		init();

		// TODO Send started-signal

		qDebug("Init finished after %d ms", time.elapsed());

		// Start threads
		workerLock_.unlock();

		workerCondition_.wakeAll();

		// Wait until all threads have started
		runningCountMutex_.lock();

		while(runningCount_ < threadCount_) {
			runningCountMutex_.unlock();
			QThread::msleep(10);
			runningCountMutex_.lock();
		}

		runningCount_ = 0;

		runningCountMutex_.unlock();

		qDebug("Threads are running after %d ms", time.elapsed());

		// Now wait for all to finish
		workerLock_.lockForWrite();

		qDebug("Threads finished after %d ms", time.elapsed());

		refreshMutex_.lock();

		if(!isInterrupted()) {
			refreshUnsafe();
		}

		qDebug("Finished after %d ms", time.elapsed());

		if(terminate_) qDebug("Terminated");
		if(interrupt_) qDebug("Interrupted");
		if(dispose_) qDebug("Disposed");

		flagMutex_.lock();

		running_ = false;
		terminate_ = false;
		interrupt_ = false;

		emit finished(turnCount);

		flagMutex_.unlock();

		refreshMutex_.unlock();

		// Running part end
		// Wait for wake up

		qDebug("In waiting position - now something may change");

		allowChangesWaitCondition_.wait(&allowChangesMutex_);
	}

	allowChangesMutex_.unlock();
	workerLock_.unlock();
	workerCondition_.wakeAll();

	qDebug("Deleting threads");

	for(int i = 0; i < threadCount_; i++) {
		threads[i]->wait();
		delete threads[i];
	}
}

void Generator::preChangeSpec() {
	// Interrupt running calculations
	specChangeMutex_.lock();
	qDebug("Spec changing - interrupting");
	interrupt();
	allowChangesMutex_.lock();
	qDebug("Got lock");
}

void Generator::postChangeSpec() {
	allowChangesMutex_.unlock();
	allowChangesWaitCondition_.wakeOne();
	qDebug("Spec changing done");

	// Make sure thread is running
	flagMutex_.lock();

	while(!running_) {
		flagMutex_.unlock();
		QThread::msleep(10);
		flagMutex_.lock();
	}

	flagMutex_.unlock();

	specChangeMutex_.unlock();
}

int Generator::threadCount() const {
	return threadCount_;
}

bool Generator::isInterrupted() const {
	return interrupt_ || dispose_;
}

bool Generator::isTerminated() const {
	return terminate_ || interrupt_ || dispose_;
}

void Generator::tryRefresh() {
	if(isRunning() && refreshMutex_.tryLock()) {
		refreshUnsafe();
		refreshMutex_.unlock();
	}
}

ImageGenerator::ImageGenerator(int threadCount, int width, int height) :
		Generator(threadCount),
		width_(width),
		height_(height),
		image_(new QImage(width, height, QImage::Format_ARGB32)) {
	data_ = (uint*) image_->bits();
	std::fill(data_, data_ + width * height, 0);
}

ImageGenerator::~ImageGenerator() {
	delete image_;
}

const QImage& ImageGenerator::image() const {
	return *image_;
}

QImage& ImageGenerator::img() {
	return *image_;
}

const QImage* ImageGenerator::imgClone() const {
	QImage* newImage = new QImage(width_, height_, QImage::Format_ARGB32);
	uint* newData = (uint*) newImage->bits();
	std::copy(data_, data_ + width_ * height_, newData);

	return newImage;
}


void ImageGenerator::setSize(int width, int height) {
	preChangeSpec();
	setSizeUnsafe(width, height);
	postChangeSpec();
}

void ImageGenerator::setSizeUnsafe(int width, int height) {
	double x0 = normX(0);
	double y0 = normY(0);

	double x1 = normX(width_);
	double y1 = normY(height_);

	width_ = width;
	height_ = height;

	QImage* oldImage = image_;

	image_ = new QImage(width, height, QImage::Format_ARGB32);
	data_ = (uint*) image_->bits();

	QPainter painter(image_);

	painter.fillRect(image_->rect(), Qt::black);

	painter.setRenderHint(QPainter::SmoothPixmapTransform);

	x0 = denormX(x0);
	y0 = denormY(y0);

	x1 = denormX(x1);
	y1 = denormY(y1);

	QRect newRect(x0, y0, x1 - x0, y1 - y0);

	painter.drawImage(newRect, *oldImage);

	delete oldImage;
}

int ImageGenerator::width() const {
	return width_;
}

int ImageGenerator::height() const {
	return height_;
}

void ImageGenerator::refreshUnsafe() {
	// Do nothing
}

void ImageGenerator::clear() {
	std::fill(data_, data_ + width() * height(), 0x0);
}

double ImageGenerator::normX(double x) const {
	int w = width();
	int h = height();

	int min = w > h ? h : w;

	return (x - (w - min) / 2.) / min;
}

double ImageGenerator::normY(double y) const {
	int w = width();
	int h = height();

	int min = w > h ? h : w;

	return (y - (h - min) / 2.) / min;
}

double ImageGenerator::denormX(double x0) const {
	int w = width();
	int h = height();

	int min = w > h ? h : w;

	return x0 * min + (w - min) / 2;
}

double ImageGenerator::denormY(double y0) const {
	int w = width();
	int h = height();

	int min = w > h ? h : w;

	return y0 * min + (h - min) / 2;
}


void ImageGenerator::rgba(int x, int y, float& r, float& g, float& b, float& a) const {
	uint argb = data_[x + y * width()];

	r = ((argb >> 16) & 0xff) / 255.f;
	g = ((argb >> 8) & 0xff) / 255.f;
	b = (argb & 0xff) / 255.f;
	a = ((argb >> 24) & 0xff) / 255.f;
}

void ImageGenerator::setRgba(int x, int y, float r, float g, float b, float a) {
	// Round
	r = r * 255 + 0.5;
	g = g * 255 + 0.5;
	b = b * 255 + 0.5;
	a = a * 255 + 0.5;

	uint argb = int(a) << 24 | int(r) << 16 | int(g) << 8 | int(b);
	data_[x + y * width()] = argb;
}
