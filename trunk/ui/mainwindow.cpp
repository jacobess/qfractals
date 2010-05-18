#include "mainwindow.h"

#include "settings.h"
#include "fractals/mandelbrot.h"

MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent)
{
	setWindowTitle(tr("QFractals"));

	init();
}

MainWindow::~MainWindow() {}

void MainWindow::init() {
	// Menu
	fileMenu = menuBar()->addMenu(tr("&File"));

	newMenu = fileMenu->addMenu(tr("&New"));
	newMandelbrotAction = newMenu->addAction("&Mandelbrot Set");
	connect(newMandelbrotAction, SIGNAL(triggered()), this, SLOT(newMandelbrot()));

	newLambdaAction = newMenu->addAction("&Lambda");
	connect(newLambdaAction, SIGNAL(triggered()), this, SLOT(newLambda()));

	newPendulumAction = newMenu->addAction("&Pendulum");
	connect(newPendulumAction, SIGNAL(triggered()), this, SLOT(newPendulum()));

	saveImageAction = fileMenu->addAction("&Save Image");
	connect(saveImageAction, SIGNAL(triggered()), this, SLOT(saveImage()));

	closeAction = fileMenu->addAction("&Close Tab");
	connect(closeAction, SIGNAL(triggered()), this, SLOT(closeTab()));

	settingsMenu = menuBar()->addMenu(tr("&Settings"));
	browseAction = settingsMenu->addAction(tr("&Browse"));
	connect(browseAction, SIGNAL(triggered(bool)), this, SLOT(browseMode(bool)));

	selectAction = settingsMenu->addAction(tr("&Select"));
	connect(selectAction, SIGNAL(triggered(bool)), this, SLOT(selectMode(bool)));

	browseAction->setCheckable(true);
	selectAction->setCheckable(true);

	selectionModeActionGroup = new QActionGroup(this);
	selectionModeActionGroup->setExclusive(true);

	selectionModeActionGroup->addAction(browseAction);
	selectionModeActionGroup->addAction(selectAction);

	browseAction->setChecked(Settings::settings()->selectionMode() == 0);
	selectAction->setChecked(Settings::settings()->selectionMode() == 1);

	// GUI Elements
	tabWidget = new QTabWidget(this);
	tabWidget->setTabsClosable(true);
	tabWidget->setTabShape(QTabWidget::Triangular);
	connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

	this->setCentralWidget(tabWidget);

	newMandelbrot();
}

void MainWindow::browseMode(bool enable) {
	Settings::settings()->setSelectionMode(enable ? 0 : 1);
}

void MainWindow::selectMode(bool enable) {
	Settings::settings()->setSelectionMode(enable ? 1 : 0);
}

void MainWindow::newMandelbrot() {
	Specification* spec = Settings::settings()->specifications()[QString("mandelbrot")];
	addTab(spec);
}

void MainWindow::newLambda() {
	Specification* spec = Settings::settings()->specifications()[QString("lambda")];
	addTab(spec);
}

void MainWindow::newPendulum() {
	Specification* spec = Settings::settings()->specifications()[QString("pendulum")];
	addTab(spec);
}

void MainWindow::saveImage(int index) {
	QWidget* widget = index == -1? tabWidget->currentWidget() : tabWidget->widget(index);

	ImageControlWidget* imgWidget = dynamic_cast<ImageControlWidget*>(widget);

	if(imgWidget != 0) {
		imgWidget->saveImage();
	}
}

void MainWindow::closeTab(int index) {
	QWidget* widget = tabWidget->widget(index);

	ImageControlWidget* imgWidget = dynamic_cast<ImageControlWidget*>(widget);

	if(imgWidget != 0) {
		tabWidget->removeTab(index);
		delete imgWidget;
	}
}

void MainWindow::addTab(Specification* spec) {
	ImageControlWidget* imgWidget = new ImageControlWidget(this, spec);
	tabWidget->addTab(imgWidget, tr("Tab"));
}
