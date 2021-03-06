#ifndef IMAGECONTROLWIDGET_H
#define IMAGECONTROLWIDGET_H

#include <QtGui>
#include <QThreadPool>
#include <QRunnable>

#include "selectablewidget.h"
#include "fractals/specification.h"

void refreshImage(Generator* generator);


class ImageControlWidget : public QWidget
{
	Q_OBJECT

	ImageGenerator* generator_;

	SelectableWidget* selectableWidget_;
	QScrollArea* scrollArea_;

	QToolButton* saveButton_;
	QToolButton* resizeButton_;
	QToolButton* editButton_;
	QLabel* statusLabel_;
	QSlider* scaleSlider_;
	QCheckBox* autoScaleCheckBox_;
	QProgressBar* progressBar_;
	QToolButton* cancelButton_;

	QHBoxLayout* hLayout_;
	QVBoxLayout* vLayout_;

	QTimer updateTimer_;
	QTimer refreshTimer_;

	int turnCount_;
	QMutex mutex_;

public:
	ImageControlWidget(QWidget* parent, const Specification& spec);
	~ImageControlWidget();

	const Specification& specification() const;

public slots:
	void saveImage();

protected:
	virtual void showEvent(QShowEvent *);
	virtual void hideEvent(QHideEvent *);

private slots:
	void setAutoScale(bool autoScale);
	void resizeEvent(QResizeEvent* event);
	void updateSize();

	void changeScale(int i);

	void setExecutionStarted(int index);
	void setExecutionStopped(int index);

	void updateProgress();
	void setStatus(QString message);
	void showResizeDialog();
	void refreshBG();

private:
	void init();
};

#endif // IMAGECONTROLWIDGET_H
