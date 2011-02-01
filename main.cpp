#include <QtGui/QApplication>
#include "ui/mainwindow.h"

#include "math/interpreter.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
