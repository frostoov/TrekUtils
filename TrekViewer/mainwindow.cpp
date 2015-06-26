#include <QMessageBox>
#include <QApplication>
#include <iostream>
#include "mainwindow.hpp"

using trek::ChamberConfig;
using trek::TrekHandler;

using std::exception;
using nlohmann::json;
using std::cout;
using std::endl;

MainWindow::MainWindow(const ChamberConfig& chamberConfig,
                       QWidget* parent)
	: QMainWindow(parent), mCurrentEvent(0) {

	mTrekHandler = new TrekHandler(chamberConfig);
	mTrekWidget  = new TrekGLWidget(mTrekHandler, this);

	createAction();
	createMenues();

	setCentralWidget(mTrekWidget);

	try {
		mTrekWidget->loadObjects();
	} catch(const exception& e) {
		errorMessage("Exception", e.what());
	}
}

MainWindow::~MainWindow() {
	delete mTrekWidget;
	delete mTrekHandler;
}

void MainWindow::keyPressEvent(QKeyEvent* ke) {
	try {
		QMainWindow::keyPressEvent(ke);
		std::set<unsigned int> triggPlanes;
		auto eventChecker = [&](const tdcdata::TUEvent & event)->bool {
			if (event.getUraganEvent().trackID != 0)
				return false;
			triggPlanes.clear();
			auto triggChambers = event.getTriggeredChambers();
			for(const auto& cham : triggChambers)
				triggPlanes.insert(mTrekHandler->getChambers().at(cham).getChamberPlane());
			if(triggPlanes.size() == 2 || ke->modifiers() == Qt::ShiftModifier)
				return true;
			else
				return false;
		};
		if(ke->key() == Qt::Key_PageDown && mCurrentEvent) {
			while(mCurrentEvent) {
				--mCurrentEvent;
				if(eventChecker(mBuffer.at(mCurrentEvent)) || ke->modifiers() == Qt::ShiftModifier)
					break;
			}
			mTrekHandler->loadEvent( mBuffer.at(mCurrentEvent) );
			mTrekHandler->createTrack();
			mTrekWidget->loadObjects();
		} else if(ke->key() == Qt::Key_PageUp && mCurrentEvent < mBuffer.size() ) {
			while(mCurrentEvent < mBuffer.size() - 1) {
				++mCurrentEvent;
				if(eventChecker(mBuffer.at(mCurrentEvent)) || ke->modifiers() == Qt::ShiftModifier)
					break;
			}
			mTrekHandler->loadEvent( mBuffer.at(mCurrentEvent) );
			mTrekHandler->createTrack();
			mTrekWidget->loadObjects();
		}
	} catch(const exception& e) {
		errorMessage("Exception", e.what());
	}
}

void MainWindow::createAction() {
	openEvtAction = new QAction("Open", this);
	quitAppAction = new QAction("Quit", this);

	openEvtAction->setShortcuts(QKeySequence::Open);
	quitAppAction->setShortcuts(QKeySequence::Quit);

	connect(openEvtAction,	&QAction::triggered,
	        this,			&MainWindow::openData);
	connect(quitAppAction,	&QAction::triggered,
	        qApp,			&QApplication::quit);
}

void MainWindow::createMenues() {
	fileMenu = new QMenu;
	fileMenu = menuBar()->addMenu("File");
	fileMenu->addAction(openEvtAction);
	fileMenu->addAction(quitAppAction);
}

void MainWindow::openData() {
	try {
		QString fileName = QFileDialog::getOpenFileName(this, "Open", 0, tr("*.tds"));
		if( fileName.isEmpty() ) return;
		mBuffer.read(fileName.toStdString());
		mTrekHandler->loadEvent( mBuffer.at(mCurrentEvent) );
		mTrekHandler->createTrack();
		mTrekWidget->loadObjects();
	} catch(const exception& e) {
		errorMessage("Exception", e.what());
	}
}

void MainWindow::errorMessage(const QString& title, const QString& message) {
	QMessageBox msg;
	msg.setText(title);
	msg.setInformativeText(message);
	msg.exec();
}
