#include "mainwindow.h"

#include "settings.h"
#include "fractals/mandelbrot.h"

MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent)
{
	setWindowTitle(tr("QFractals"));

	ColorPalette palette;
	palette.a().add(0, 1);

	palette.r().add(0, 0);
	palette.g().add(0, 0);
	palette.b().add(0, 0);

	palette.r().add(26, 1);
	palette.g().add(25, 0.6);
	palette.b().add(24, 0.25);

	palette.r().add(52, 1);
	palette.g().add(50, 1);
	palette.b().add(48, 1);

	palette.r().add(78, 0.2);
	palette.g().add(75, 0.2);
	palette.b().add(72, 0.8);

	palette.r().setPeriod(104);
	palette.g().setPeriod(100);
	palette.b().setPeriod(96);

	QList< Interpreter<long double> > base;

	Interpreter<long double> iteration;

	iteration.addRROp(QString("sqr"), -1, -1);
	iteration.addRROp(QString("add_c"), -1, -1);

	generator_ = Mandelbrot<long double>(
			Transformation<long double>(4, 0, 0, 4, -2, -2),
			base,
			iteration,
			10240,
			2,
			1e-6,
			palette).createGenerator(640, 480);

	initMenu();
	initGUI();
	initConnections();

	//imgGenController = new generator_ControlWidget(this, generator_);


	generator_->start();
}

MainWindow::~MainWindow()
{
	generator_->cancelWait();
	delete generator_;
	//delete imgGenController;
}

void MainWindow::initMenu() {
	/*fileMenu = menuBar()->addMenu(tr("&File"));
	saveImageAction = fileMenu->addAction(tr("&Save Image"));

	viewMenu = menuBar()->addMenu(tr("&View"));
	fullScreenAction = viewMenu->addAction(tr("&Full Screen"));

	imageMenu = menuBar()->addMenu(tr("&Image"));
	resizeAction = imageMenu->addAction(tr("&Resize"));
	imageMenu->addSeparator();
	mandelbrotAction = imageMenu->addAction(tr("&Mandelbrot"));
	magneticPendulumAction = imageMenu->addAction(tr("Magnetic &Pendulum"));
	ifsAction = imageMenu->addAction(tr("&IFS"));*/

	settingsMenu = menuBar()->addMenu(tr("&Settings"));
	browseAction = settingsMenu->addAction(tr("&Browse"));
	selectAction = settingsMenu->addAction(tr("&Select"));

	browseAction->setCheckable(true);
	selectAction->setCheckable(true);

	selectionModeActionGroup = new QActionGroup(this);
	selectionModeActionGroup->setExclusive(true);

	selectionModeActionGroup->addAction(browseAction);
	selectionModeActionGroup->addAction(selectAction);

	browseAction->setChecked(Settings::settings()->selectionMode() == 0);
	selectAction->setChecked(Settings::settings()->selectionMode() == 1);
}

void MainWindow::initGUI() {
	imgWidget = new ImageControlWidget(this, generator_);
	this->setCentralWidget(imgWidget);
}

void MainWindow::initConnections() {
	//connect(saveImageAction, SIGNAL(triggered()), this, SLOT(saveImage()));

	connect(browseAction, SIGNAL(triggered(bool)), this, SLOT(browseMode(bool)));
	connect(selectAction, SIGNAL(triggered(bool)), this, SLOT(selectMode(bool)));

	//connect(ifsAction, SIGNAL(triggered()), this, SLOT(ifs()));
	//connect(magneticPendulumAction, SIGNAL(triggered()), this, SLOT(magneticPendulum()));
	//connect(mandelbrotAction, SIGNAL(triggered()), this, SLOT(mandelbrot()));
}

void MainWindow::saveImage() {
	// TODO: Check whether still running and warn
	QString fileName = QFileDialog::getSaveFileName(this,
	     tr("Save Image"), "", tr("Image Files (*.png)"));

	if(!fileName.isEmpty()) {
		generator_->image().save(fileName, "PNG");
		// Provide error message
	}
}

void MainWindow::fullScreen() {
}

void MainWindow::resizeImage() {

}

void MainWindow::browseMode(bool enable) {
	Settings::settings()->setSelectionMode(enable ? 0 : 1);
}

void MainWindow::selectMode(bool enable) {
	Settings::settings()->setSelectionMode(enable ? 1 : 0);
}
