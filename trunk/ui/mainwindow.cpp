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

	foreach(Specification* spec , Settings::settings()->specifications()) {
		newMenu->addAction(spec->description());
	}

	connect(newMenu, SIGNAL(triggered(QAction*)), SLOT(addNew(QAction*)));

	newMenu->addSeparator();

	saveImageAction = fileMenu->addAction("&Save Image");
	connect(saveImageAction, SIGNAL(triggered()), this, SLOT(saveImage()));

	duplicateAction = fileMenu->addAction("&Duplicate Tab");
	connect(duplicateAction, SIGNAL(triggered()), this, SLOT(duplicateTab()));

	closeAction = fileMenu->addAction("&Close Tab");
	connect(closeAction, SIGNAL(triggered()), this, SLOT(closeTab()));

	settingsMenu = menuBar()->addMenu(tr("&Settings"));
	navigationMenu = settingsMenu->addMenu(tr("&Navigation Mode"));

	browseAction = navigationMenu->addAction(tr("&Browse"));
	connect(browseAction, SIGNAL(triggered(bool)), this, SLOT(browseMode(bool)));

	selectAction = navigationMenu->addAction(tr("&Select"));
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

	addTab(*Settings::settings()->specifications().first());
}

void MainWindow::browseMode(bool enable) {
	Settings::settings()->setSelectionMode(enable ? 0 : 1);
}

void MainWindow::selectMode(bool enable) {
	Settings::settings()->setSelectionMode(enable ? 1 : 0);
}

void MainWindow::addNew(QAction* action) {
	foreach(Specification* spec , Settings::settings()->specifications()) {
		if(spec->description() == action->text()) {
			addTab(*spec);
		}
	}
}

void MainWindow::saveImage(int index) {
	QWidget* widget = index == -1? tabWidget->currentWidget() : tabWidget->widget(index);

	ImageControlWidget* imgWidget = dynamic_cast<ImageControlWidget*>(widget);

	if(imgWidget != 0) {
		imgWidget->saveImage();
	}
}

void MainWindow::duplicateTab(int index) {
	QWidget* widget = index == -1? tabWidget->currentWidget() : tabWidget->widget(index);

	ImageControlWidget* imgWidget = dynamic_cast<ImageControlWidget*>(widget);

	if(imgWidget != 0) {
		addTab(imgWidget->specification());
	}
}

void MainWindow::closeTab(int index) {
	QWidget* widget = index == -1? tabWidget->currentWidget() : tabWidget->widget(index);

	ImageControlWidget* imgWidget = dynamic_cast<ImageControlWidget*>(widget);

	if(imgWidget != 0) {
		tabWidget->removeTab(index);
		delete imgWidget;
	}
}

void MainWindow::addTab(const Specification& spec) {
	ImageControlWidget* imgWidget = new ImageControlWidget(this, spec);
	tabWidget->addTab(imgWidget, spec.description());
	tabWidget->setCurrentWidget(imgWidget);
}
