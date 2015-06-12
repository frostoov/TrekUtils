#include <QApplication>
#include <QMessageBox>
#include "configparser/chamberconfigparser.hpp"
#include "configparser/appconfigparser.hpp"
#include "mainwindow.hpp"

using std::cout;
using std::endl;
using std::exception;

void errorMessage(const QString& title, const QString& message) {
	QMessageBox msg;
	msg.setText(title);
	msg.setInformativeText(message);
	msg.exec();
}

int main(int argc, char* argv[]) {
	std::ios_base::sync_with_stdio(false);
	ChamberConfigParser chambersParser;
	try {
		chambersParser.load("chambers.json");
	} catch (const exception& e) {
		QApplication a(argc, argv);
		errorMessage("Exception", QString("Chambers Config: ") + e.what());
		return a.exec();
	}
	AppConfigParser appParser;
	try {
		appParser.load("TrekViewer.json");
	} catch (const exception& e) {
		QApplication a(argc, argv);
		errorMessage("Exception", QString("Chambers Config: ") + e.what());
		return a.exec();
	}
	QApplication a(argc, argv);
	MainWindow w(chambersParser.getConfig(), appParser.getConfig());
	w.show();
	return a.exec();
}
