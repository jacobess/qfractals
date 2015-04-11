#ifndef IMAGECONTROLWIDGET_H
#define IMAGECONTROLWIDGET_H

#include <QtWidgets>
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
	QProgressBar* progressBar_;
	QToolButton* cancelButton_;

	QHBoxLayout* hLayout_;
	QVBoxLayout* vLayout_;
public:
	ImageControlWidget(QWidget* parent, Generator* generator = 0);

	void setGenerator(Generator* generator);

private slots:
	void changeScale(int i);

	void setStarted();
	void updateProgress(int, int);
	void setDone(bool cancelled);
	void setStatus(QString message);
	void showResizeDialog();
	void saveImage();

private:
	void initGUI();
	void initConnections();
};

#endif // IMAGECONTROLWIDGET_H
