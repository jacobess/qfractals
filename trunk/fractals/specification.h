#ifndef SPECIFICATION_H
#define SPECIFICATION_H

#include <QThread>
#include <QTimer>
#include <QImage>
#include <QMutex>

#include <QTime>

#include "graphics/image.h"

class Generator;

class Specification {
public:
	virtual Generator* createGenerator(int width, int height) const = 0;
};

class Thread : public QThread {
	const int index_;
	Generator& parent_;
public:
	Thread(int index, Generator& parent); // Obtains a ticket from parent
	void run(); // Calls execute in parent
};

class Generator : public QObject {
	Q_OBJECT

	bool isStopped_;

	int runningCount_;

	bool isSelectable_;

	QList<Thread*> threads_;

	QMutex mutex_;

public:
	Generator(int threadCount, bool isSelectable);
	~Generator();

	virtual const Specification& specification() const = 0;

	bool isRunning() const; // runningCount_ > 0?

	virtual int progress() const = 0;
	virtual int totalSteps() const = 0;

	bool isSelectable() const;

	// This method may trigger updates
	// in image containers
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

	// TODO more detailed point description

signals:
	void started();
	void done(bool cancelled);

	void resized(int width, int height);

public slots:
	void start();
	void cancel();
	void cancelWait();

	void refresh();

protected:
	virtual Image& img() = 0;
	virtual const Image& img() const = 0;

	virtual Specification& specification() = 0;

	/** Threads have to check this regularly and must terminate immediately
	  * if this function returns true */
	bool isStopped() const;

	/** Cancels tasks (if running) and waits for termination, Not thread-safe */
	void cancelWaitUnsafe();

	/** Starts threads. Not thread-safe */
	void startUnsafe();

	int threadCount() const;

	/** Some initialization code for subclasses */
	virtual void init() = 0;

	/** Method called by the index-th thread */
	virtual void exec(int index) = 0;

	QMutex threadMutex_;

private:
	void run(int i);

friend class Thread;
};

#endif // SPECIFICATION_H
