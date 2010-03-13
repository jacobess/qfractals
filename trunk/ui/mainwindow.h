#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>

#include "fractals/specification.h"
#include "ui/imagecontrolwidget.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

	QMenu* fileMenu;
	QAction* saveImageAction;

	QMenu* viewMenu;
	QAction* fullScreenAction;

	QMenu* imageMenu;
	QAction* resizeAction;
	QAction* mandelbrotAction;
	QAction* magneticPendulumAction;
	QAction* ifsAction;
	QAction* buddhabrotAction;

	QMenu* settingsMenu;
	QAction* browseAction;
	QAction* selectAction;

	QActionGroup* selectionModeActionGroup;

	ImageControlWidget* imgWidget;

	Generator* generator_;

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void saveImage();
	void fullScreen();
	void resizeImage();
	void browseMode(bool enable);
	void selectMode(bool enable);

private:
	void initMenu();
	void initGUI();
	void initConnections();
	void setGenerator(Generator* generator);
};

#endif // MAINWINDOW_H
