#ifndef SPECIFICATION_H
#define SPECIFICATION_H

#include <QThread>
#include <QImage>
#include <QMutex>
#include <QWaitCondition>
#include <QTime>
#include <QRunnable>
#include <QThreadPool>
#include <QReadWriteLock>
#include <QTimer>

// First, the thread-stuff

class Generator;

// On RefreshThreads:
/*
 Using a refreshThread has several advantages:

 1. no multiple calls to refresh when using waitConditions
 2. easier synchronization
 3. no lengthy processes in main thread
 */

class WorkingThread : public QThread {
	Generator& parent_;

	WorkingThread(Generator& parent); // can be private since only called by generator

protected:
	void run();

friend class Generator;
};

class Generator : public QThread {
	Q_OBJECT

	int threadCount_;

	QMutex flagMutex_;
	bool running_;
	bool interrupt_;
	bool terminate_;
	bool dispose_;

	int waitingCount_;
	QMutex waitingCountMutex_;

	QWaitCondition waitCondition_;

	QReadWriteLock workerLock_;
	QWaitCondition workerCondition_;

	QWaitCondition mainWaitCondition_;
	QMutex mainMutex_;

	QMutex specChangeMutex_;

	QMutex refreshMutex_;

public:
	Generator(int threadCount);
	~Generator();

	// public access to thread stuff
	virtual int progress() const = 0;
	virtual int totalSteps() const = 0;

	bool isRunning() const;

public slots:
	void terminate();
	void tryRefresh();

signals:
	void started();
	void finished();

protected:
	// Starts the main thread
	void run();
	void interrupt();

	int threadCount() const;

	/** Threads have to check this regularly and must terminate immediately
	  * if this function returns true */
	bool isInterrupted() const;
	bool isTerminated() const;

	void preChangeSpec();
	void postChangeSpec();

	/** Some initialization code */
	virtual void init() = 0;

	/** Method called by the index-th thread */
	virtual void exec(int index) = 0;

	virtual void refreshUnsafe() = 0;

friend class WorkingThread;
};

// Now for the image stuff:
class ImageGenerator;

class Specification {
public:
	virtual ImageGenerator* createGenerator(int width, int height) const = 0;
	virtual QString title() const { return "TODO"; }
};

class ImageGenerator : public Generator {
	Q_OBJECT

	int width_;
	int height_;
	QImage* image_;

	uint* data_;

public:
	ImageGenerator(int threadCount, int width, int height);
	virtual ~ImageGenerator();

	virtual const Specification& specification() const = 0;

	// Image part
	const QImage& image() const;

	int width() const;
	int height() const;

	virtual void setSize(int width, int height);
	virtual void clear();

signals:
	void resized(int width, int height);

protected:
	virtual Specification& spec() = 0;
	QImage& img();

	virtual void setSizeUnsafe(int width, int height);
	virtual void refreshUnsafe();

	// Transform into normalized coordinates independent of width and height
	double normX(double x) const;
	double normY(double y) const;

	double denormX(double x0) const;
	double denormY(double y0) const;

	void rgba(int x, int y, float& r, float& g, float& b, float& a) const;
	void setRgba(int x, int y, float r, float g, float b, float a);
};

#endif // SPECIFICATION_H
