#include "imagecontrolwidget.h"

#include "settings.h"
#include "resizedialog.h"

#define ABS(x)	((x) >= 0 ? (x) : -(x))

ImageControlWidget::ImageControlWidget(QWidget* parent, Generator* generator) :
		QWidget(parent)
{
	initGUI();
	setGenerator(generator);
}

void ImageControlWidget::initGUI() {
	selectableWidget_ = new SelectableWidget(this);

	scrollArea_ = new QScrollArea(this);
	scrollArea_->setAlignment(Qt::AlignCenter);
	scrollArea_->setWidget(selectableWidget_);

	saveButton_ = new QToolButton(this);
	saveButton_->setText("Save");

	resizeButton_ = new QToolButton(this);
	resizeButton_->setText("Resize");

	editButton_ = new QToolButton(this);
	editButton_->setText("Edit");
	editButton_->setEnabled(false);

	progressBar_ = new QProgressBar(this);

	cancelButton_ = new QToolButton(this);
	cancelButton_->setText("Cancel");

	statusLabel_ = new QLabel(this);
	int prec = Settings::settings()->coordinatesPrecision();

	QString sx = QString("%1").arg(0., 1, 'f', prec);
	QString sy = QString("%1").arg(0., 1, 'f', prec);

	statusLabel_->setText(QString("%1 | %2").arg(sx).arg(sy));


	scaleSlider_ = new QSlider(Qt::Horizontal, this);
	scaleSlider_->setRange(-Settings::settings()->scaleSliderTicks(),
			       Settings::settings()->scaleSliderTicks());
	scaleSlider_->setValue(0);

	hLayout_ = new QHBoxLayout;

	hLayout_->addWidget(saveButton_);
	hLayout_->addWidget(resizeButton_);
	hLayout_->addWidget(editButton_);
	hLayout_->addSpacing(12);
	hLayout_->addWidget(scaleSlider_);
	hLayout_->addSpacing(12);
	hLayout_->addWidget(statusLabel_);
	hLayout_->addSpacing(12);
	hLayout_->addWidget(progressBar_);
	hLayout_->addWidget(cancelButton_);

	vLayout_ = new QVBoxLayout;
	vLayout_->addWidget(scrollArea_);
	vLayout_->addLayout(hLayout_);

	vLayout_->setMargin(0);
	vLayout_->setSpacing(0);

	hLayout_->setMargin(9);
	hLayout_->setSpacing(9);

	setLayout(vLayout_);

	connect(resizeButton_, SIGNAL(clicked()), this, SLOT(showResizeDialog()));
	connect(scaleSlider_, SIGNAL(valueChanged(int)), this, SLOT(changeScale(int)));
	connect(selectableWidget_, SIGNAL(status(QString)), this, SLOT(setStatus(QString)));
	connect(saveButton_, SIGNAL(clicked()), this, SLOT(saveImage()));
}

void ImageControlWidget::setGenerator(Generator* generator) {
	// Disconnect old one from cancel button and this
	Generator* oldGen = generator_;
	generator_ = generator;

	if(oldGen != 0) {
		disconnect(oldGen, 0, this, 0);
		disconnect(cancelButton_, 0, oldGen, 0);
	}

	selectableWidget_->setGenerator(generator);

	if(generator != 0) {
		// First receive done-signal
		connect(generator_, SIGNAL(done(bool)), this, SLOT(setDone(bool)), Qt::DirectConnection);
		connect(generator_, SIGNAL(started()), this, SLOT(setStarted()), Qt::DirectConnection);
		connect(generator_, SIGNAL(updated(int, int)), this, SLOT(updateProgress(int,int)));

		connect(cancelButton_, SIGNAL(clicked()), generator_, SLOT(cancel()));
	} else {
		// TODO: Deactivate everything
	}
}

void ImageControlWidget::changeScale(int i) {
	int w = generator_->width();
	int h = generator_->height();

	int ticks = Settings::settings()->scaleSliderTicks();

	qreal max = Settings::settings()->maxScaleFactor();

	int wp = scrollArea_->maximumViewportSize().width();
	int hp = scrollArea_->maximumViewportSize().height();

	// Which one comes without scrollbar?
	if(wp * h > w * hp) {
		wp -= scrollArea_->verticalScrollBar()->width();
	} else if(wp * h < w * hp) {
		hp -= scrollArea_->horizontalScrollBar()->height();
	}

	qreal iw = ticks * (log(wp) - log(w)) / log(max);
	qreal ih = ticks * (log(hp) - log(h)) / log(max);

	// Which is closest, iw, ih or 0?
	qreal d = ABS(i);
	char j = 'M';

	if(ABS(iw - i) < d) {
		j = 'W';
		d = ABS(iw - i);
	}

	if(ABS(ih - i) < d) {
		j = 'H';
		d = ABS(ih - i);
	}

	qreal scale;

	if(d <= ticks / 32) {
		if(j == 'M') {
			scale = 1;

			selectableWidget_->resize(w, h);
			scaleSlider_->setValue(0);
		} else if(j == 'W') {
			scale = wp;
			scale /= w;

			h = h * wp / w;
			w = wp;

			selectableWidget_->resize(w, h);
			scaleSlider_->setValue(iw);
		} else {
			scale = hp;
			scale /= h;

			w = w * hp / h;
			h = hp;

			selectableWidget_->resize(w, h);
			scaleSlider_->setValue(ih);
		}
	} else {
		qreal e = qreal(i) / ticks;
		scale = pow(max, e);

		selectableWidget_->resize(w * scale, h * scale);
	}

	scaleSlider_->setToolTip(tr("%1 %").arg(scale * 100, 0, 'f', 2));
}

void ImageControlWidget::setStarted() {
	qDebug("Receiving started");

	cancelButton_->setText("Cancel");
	disconnect(cancelButton_, 0, generator_, 0);
	connect(cancelButton_, SIGNAL(clicked()), generator_, SLOT(cancel()));

	progressBar_->setEnabled(generator_->isRunning());
}

void ImageControlWidget::updateProgress(int progress, int totalSteps) {
	if(progressBar_->maximum() != totalSteps) {
		progressBar_->setMaximum(totalSteps);
	}

	progressBar_->setValue(progress);
}

void ImageControlWidget::setDone(bool) {
	qDebug("Received done-signal");

	cancelButton_->setText("Resume");
	disconnect(cancelButton_, 0, generator_, 0);
	connect(cancelButton_, SIGNAL(clicked()), generator_, SLOT(start()));

	progressBar_->setEnabled(false);
}

void ImageControlWidget::setStatus(QString message) {
	statusLabel_->setText(message);
}

void ImageControlWidget::showResizeDialog() {
	ResizeDialog dialog(this, generator_->width(), generator_->height());
	int ret = dialog.exec();

	if(ret == QDialog::Accepted) {
		generator_->setSize(dialog.width(), dialog.height());

		// update size
		changeScale(scaleSlider_->value());
	}
}

void ImageControlWidget::saveImage() {
	bool save = true;
	if(generator_->isRunning()) {
		QMessageBox msgBox;
		msgBox.setText("Calculation is still running.");
		msgBox.setInformativeText("Do you want to save anyway?");
		msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
		msgBox.setDefaultButton(QMessageBox::Yes);
		int ret = msgBox.exec();

		save = (ret == QMessageBox::Yes);
	}

	if(save) {
		QString fileName = QFileDialog::getSaveFileName(this,
		     tr("Save Image"), "", tr("Image Files (*.png)"));

		if(!fileName.isEmpty()) {
			// TODO: Does not work!
			if(!generator_->image().save(fileName, "PNG")) {
				qDebug("Could not save file");
				// Provide error message
				QMessageBox msgBox;
				msgBox.setText("Error saving image.");
				msgBox.setInformativeText("Could not save image.");
				msgBox.setStandardButtons(QMessageBox::Ok);
				msgBox.setDefaultButton(QMessageBox::Ok);
				msgBox.exec();
			}
		}
	}
}
