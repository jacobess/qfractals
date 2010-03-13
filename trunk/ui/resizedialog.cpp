#include "resizedialog.h"

ResizeDialog::ResizeDialog(QWidget *parent, int width, int height) :
		QDialog(parent),
		ratio_(qreal(width) / qreal(height)),
		lastFocus_('w') {

	// TODO Make a border with title image dimensions
	// and connect size of buttons

	setWindowTitle(tr("Resize Image"));

	widthLabel_ = new QLabel("Width: ", this);

	heightLabel_ = new QLabel("Height: ", this);

	widthSpinBox_ = new QSpinBox(this);
	widthSpinBox_->setRange(8, 10240);
	widthSpinBox_->setValue(width);
	widthSpinBox_->setSingleStep(10);

	heightSpinBox_ = new QSpinBox(this);
	heightSpinBox_->setRange(8, 10240);
	heightSpinBox_->setValue(height);
	heightSpinBox_->setSingleStep(10);

	keepRatioCheckBox_ = new QCheckBox("Keep ratio", this);
	keepRatioCheckBox_->setChecked(false);

	okButton_ = new QPushButton("Ok", this);
	cancelButton_ = new QPushButton("Cancel", this);

	okButton_->sizeHint();

	topLayout_ = new QGridLayout;

	topLayout_->addWidget(widthLabel_, 0, 0, 1, 1, Qt::AlignRight);
	topLayout_->addWidget(widthSpinBox_, 0, 1, 1, 1, Qt::AlignLeft);
	topLayout_->addWidget(heightLabel_, 1, 0, 1, 1, Qt::AlignRight);
	topLayout_->addWidget(heightSpinBox_, 1, 1, 1, 1, Qt::AlignLeft);

	topFrame_ = new QFrame(this);
	topFrame_->setLayout(topLayout_);

	topFrame_->setFrameStyle(QFrame::Box | QFrame::Sunken);

	buttonsLayout_ = new QHBoxLayout;

	// TODO insert line here
	buttonsLayout_->addWidget(keepRatioCheckBox_);

	buttonsLayout_->addStretch(2);
	buttonsLayout_->addWidget(cancelButton_);
	buttonsLayout_->addWidget(okButton_);

	totalLayout_ = new QVBoxLayout(this);

	totalLayout_->addWidget(topFrame_);
	totalLayout_->addLayout(buttonsLayout_);

	connect(okButton_, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(reject()));

	connect(widthSpinBox_, SIGNAL(valueChanged(int)), this, SLOT(widthChanged(int)));
	connect(heightSpinBox_, SIGNAL(valueChanged(int)), this, SLOT(heightChanged(int)));
	connect(keepRatioCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(keepRatioChecked(int)));
}

ResizeDialog::~ResizeDialog() {
	delete widthLabel_;
	delete heightLabel_;
	delete widthSpinBox_;
	delete heightSpinBox_;
	delete okButton_;
	delete cancelButton_;
	delete buttonsLayout_;
	delete topLayout_;
	delete totalLayout_;
}

int ResizeDialog::width() {
	return widthSpinBox_->value();
}

int ResizeDialog::height() {
	return heightSpinBox_->value();
}

void ResizeDialog::widthChanged(int width) {
	lastFocus_ = 'w';

	if(keepRatioCheckBox_->isChecked()) {
		heightSpinBox_->setValue(width / ratio_ + 0.5);
	}
}

void ResizeDialog::heightChanged(int height) {
	lastFocus_ = 'h';

	if(keepRatioCheckBox_->isChecked()) {
		widthSpinBox_->setValue(height * ratio_ + 0.5);
	}
}

void ResizeDialog::keepRatioChecked(int state) {
	if(state == Qt::Checked) {
		if(lastFocus_ == 'h') {
			widthSpinBox_->setValue(heightSpinBox_->value() * ratio_);
		} else {
			heightSpinBox_->setValue(widthSpinBox_->value() / ratio_);
		}
	}
}
