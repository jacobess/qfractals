#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>

#include "fractals/specification.h"
#include "ui/imagecontrolwidget.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

	QMenu* fileMenu;
	QMenu* newMenu;

	QAction* newMandelbrotAction;
	QAction* newLambdaAction;
	QAction* newTricornAction;
	QAction* newBurningShipAction;
	QAction* newMagnet1Action;
	QAction* newFernAction;
	QAction* newPendulumAction;
	QAction* saveImageAction;
	QAction* duplicateAction;
	QAction* closeAction;

	QMenu* settingsMenu;
	QMenu* navigationMenu;
	QAction* browseAction;
	QAction* selectAction;

	QActionGroup* selectionModeActionGroup;

	QTabWidget* tabWidget;

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void browseMode(bool enable);
	void selectMode(bool enable);

	void newMandelbrot();
	void newLambda();
	void newTricorn();
	void newBurningShip();
	void newMagnet1();
	void newFern();
	void newPendulum();

	void saveImage(int tab = -1);
	void closeTab(int tab = -1);
	void duplicateTab(int tab = -1);

	void addTab(const Specification& spec);

private:
	void init();
};

#endif // MAINWINDOW_H
