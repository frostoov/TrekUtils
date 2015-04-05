#include <QApplication>
#include "mainwindow.hpp"

int main(int argc,char* argv[]) {
	std::ios_base::sync_with_stdio(false);
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();

}
