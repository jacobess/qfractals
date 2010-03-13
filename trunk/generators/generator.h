#ifndef GENERATOR_H
#define GENERATOR_H

#include <QObject>
#include <QImage>
#include <QMutex>
#include <QTimer>
#include <QThread>

class Generator;

class Thread : public QThread {
	const int index_;
	Generator& parent_;
public:
	Thread(int index, Generator& parent); // Obtains a ticket from parent
	void run(); // Calls execute in parent
};

class Generator : public QObject {
	Q_OBJECT

	QTimer timer_;

	bool isStopped_;

	int count_;
	int runningCount_; // in startUnsafe this is set to count

	QList<Thread*> threads_;

	QMutex threadMutex;

public:
	Generator();
	~Generator();

	bool isRunning() const; // runningCount_ > 0?

	virtual const QImage& image() const = 0;

	// This method may trigger updates
	// in image containers
	virtual const QImage& image() = 0;

signals:
	void started();
	void done(bool cancelled);

	void updated(int progress, int totalSteps);

public slots:
	void start();
	void cancel();

protected:
	void startUnsafe();

	void cancelAndWaitUnsafe();

	/** Some initialization code for subclasses */
	virtual void init() = 0;

	/** Threads have to check this regularly and must terminate immediately
	  * if this function returns true */
	bool isStopped() const;

	/** Method called by the index-th thread */
	virtual void exec(int index, int count) = 0;

	int progress_;
	int totalSteps_;

	QMutex mutex;

protected slots:
	/** Subclasses can use this method to set the progress. 0, 0 means
	  * "indetermined" and is the default */
	void setProgress(int progress, int totalSteps);

	/** Sends the update(int progress, int totalSteps)-message */
	void emitUpdate();

private:
	void run(int i);

friend class Thread;
};

#endif // GENERATOR_H
