#include <QApplication>
#include <QMessageBox>
#include "configparser/chamberconfigparser.hpp"
#include "configparser/appconfigparser.hpp"
#include "mainwindow.hpp"

using std::cout;
using std::endl;
using std::flush;
using std::exception;

void errorMessage(const QString& title, const QString& message) {
	QMessageBox msg;
	msg.setText(title);
	msg.setInformativeText(message);
	msg.exec();
}

struct AppConfig {
	uint32_t defaultOffset;
	double   defaultSpeed;
};

int main(int argc, char* argv[]) {
	std::ios_base::sync_with_stdio(false);

	AppConfig appConfig{0, 0};

	AppConfigParser appParser{{
		{"speed", 0},
		{"offset", 0},
	}};
	try {
		cout << "Reading TrekViewer.conf: " << flush;
		appParser.load("TrekViewer.conf");
		appConfig.defaultOffset = appParser.getConfig().at("offset");
		appConfig.defaultSpeed  = appParser.getConfig().at("speed");
		cout << "success" << endl;
	} catch (const exception& e) {
		QApplication a(argc, argv);
		errorMessage("Exception", QString("TrekViewer.conf: ") + e.what());
		return a.exec();
	}

	ChamberConfigParser chambersParser(appConfig.defaultOffset, appConfig.defaultSpeed);
	try {
		cout << "Reading chambers.conf: " << flush;
		chambersParser.load("chambers.conf");
		cout << "success" << endl;
	} catch (const exception& e) {
		QApplication a(argc, argv);
		errorMessage("Exception", QString("chambers.conf: ") + e.what());
		return a.exec();
	}

	QApplication a(argc, argv);
	MainWindow w(chambersParser.getConfig());
	w.show();
	return a.exec();
}
