#ifndef RESIZEDIALOG_H
#define RESIZEDIALOG_H

#include <QtGui>

class ResizeDialog : public QDialog
{
	Q_OBJECT

	QLabel* widthLabel_;
	QSpinBox* widthSpinBox_;
	QLabel* heightLabel_;
	QSpinBox* heightSpinBox_;

	QPushButton* okButton_;
	QPushButton* cancelButton_;

	QCheckBox* keepRatioCheckBox_;

	QFrame* topFrame_;

	QGridLayout* topLayout_;
	QHBoxLayout* buttonsLayout_;
	QVBoxLayout* totalLayout_;

	qreal ratio_;

	char lastFocus_;

public:
	ResizeDialog(QWidget* parent, int width, int height);
	~ResizeDialog();

	int width();
	int height();

private slots:
	void widthChanged(int width);
	void heightChanged(int height);
	void keepRatioChecked(int state);
};

#endif // RESIZEDIALOG_H
