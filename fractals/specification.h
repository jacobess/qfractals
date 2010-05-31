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

#include "graphics/image.h"

class Generator;

class Specification {
public:
	virtual Generator* createGenerator(int width, int height) const = 0;
};

class SubThread;

class Generator : public QObject {
	Q_OBJECT

	const bool isSelectable_;
	const int threadCount_;

	QThreadPool threadPool_;

	// Used to control the state the threads
	bool restart_;
	bool abort_;
	bool dispose_;

	QMutex abortMutex_;
	QReadWriteLock rwLock_;
	QWaitCondition waitCondition_;

	QMutex refreshMutex_;

	QMutex notRunningCountMutex_;
	int notRunningCount_;


	QMutex executionCountMutex_;
	int executionCount_;

public:
	Generator(int threadCount, bool isSelectable);
	~Generator();

	virtual const Specification& specification() const = 0;

	virtual int progress() const = 0;
	virtual int totalSteps() const = 0;

	bool isSelectable() const;

	const QImage& image() const;

	int width() const;
	int height() const;

	void setSize(int width, int height);

	qreal normX(qreal x) const;
	qreal normY(qreal y) const;

	qreal denormX(qreal x0) const;
	qreal denormY(qreal y0) const;

	virtual void scale(int cx, int cy, qreal factor) = 0;
	virtual void move(int dx, int dy) = 0;
	virtual void select(qreal wx, qreal wy, qreal hx, qreal hy, qreal x0, qreal y0) = 0;

	virtual QString pointDescription(qreal x, qreal y) = 0;

signals:
	void executionStarted(int);
	void executionStopped(int);

	void resized(int width, int height);

public slots:
	void launch();

	void resume();
	void abort();

	void refresh();

	void dispose();

protected:
	int threadCount() const;

	void lockForCommit(bool restart = false, bool dispose = false); // this locks runningMutex_
	void finishCommit(); // this frees runningMutex_

	bool isAborted();

	virtual Image& img() = 0;
	virtual const Image& img() const = 0;

	virtual Specification& spec() = 0;

	/** Threads have to check this regularly and must terminate immediately
	  * if this function returns true */
	bool isAborted() const;

	/** Some initialization code for subclasses */
	virtual void init() = 0;

	/** Method called by the index-th thread */
	virtual void exec(int index) = 0;

private:
	void emitStopped();
	void emitStarted();

friend class SubThread;
};

class SubThread : public QRunnable {
	Generator& parent_;
	const int index_;

public:
	SubThread(Generator& parent, int index);
protected:
	void run();

friend class Generator;
};

#endif // SPECIFICATION_H
