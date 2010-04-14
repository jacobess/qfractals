#ifndef IMAGECONTROLWIDGET_H
#define IMAGECONTROLWIDGET_H

#include <QtGui>
#include "selectablewidget.h"
#include "fractals/specification.h"

// TODO: Resize dialog
// Unify cancel and resume-button

class ImageControlWidget : public QWidget
{
	Q_OBJECT

	Generator* generator_;

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

public:
	ImageControlWidget(QWidget* parent, Generator* generator = 0);

	void setGenerator(Generator* generator);

private slots:
	void setAutoScale(bool autoScale);
	void resizeEvent(QResizeEvent* event);
	void updateSize();

	void changeScale(int i);

	void setStarted();
	void updateProgress();
	void setDone(bool cancelled);
	void setStatus(QString message);
	void showResizeDialog();
	void saveImage();

private:
	void initGUI();
	void initConnections();
};

#endif // IMAGECONTROLWIDGET_H
