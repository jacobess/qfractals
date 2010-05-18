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
	QAction* newPendulumAction;
	QAction* saveImageAction;
	QAction* closeAction;

	QMenu* settingsMenu;
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
	void newPendulum();
	void saveImage(int tab = -1);
	void closeTab(int tab);

	void addTab(Specification* spec);

private:
	void init();
};

#endif // MAINWINDOW_H
