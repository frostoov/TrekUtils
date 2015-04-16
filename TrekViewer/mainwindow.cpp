#include <QMessageBox>
#include <iostream>
#include "mainwindow.hpp"

using std::exception;

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), mCurrentEvent(0) {
	ChamberConfigParser chamberParser;
	try {
		chamberParser.load("chambers.conf");
		mChamberConfig = chamberParser.getConfig();
	} catch( const exception& e) {
		errorMessage("Exception", QString("Chambers Config: ") + e.what());
	}

	double speed = 0;
	uint32_t pedestal = 0;

	AppConfigParser appParser({{"pedestal", "0"} , {"speed", "0"}});
	try {
		appParser.load("TUVisualization.conf");
		mAppConfig     = appParser.getConfig();
		try {
			speed    = std::stod(mAppConfig.at("speed"));
			pedestal = std::stoul(mAppConfig.at("pedestal"));
		} catch(const exception& e) {
			errorMessage("Exception", QString("TUVisualization Config: ") + e.what());
		}
	} catch(const exception& e) {
		errorMessage("Exception", QString("TUVisualization Config: ") + e.what());
	}

	mTrekHandler = new TrekHandler(chamberParser.getConfig(), pedestal, speed);
	mTrekWidget  = new TrekGLWidget(mTrekHandler,this);

	QSurfaceFormat glf;
	glf.setSamples(16);
	glf.setDepthBufferSize(24);
	glf.setStencilBufferSize(8);
	glf.setVersion(2, 1);
	glf.setProfile(QSurfaceFormat::CoreProfile);
	glf.setRenderableType(QSurfaceFormat::OpenGL);
	glf.setSwapBehavior(QSurfaceFormat::TripleBuffer);
	mTrekWidget->setFormat(glf);

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
		auto eventChecker = [&](const tdcdata::TUEvent& event)->bool {
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
	openEvtAction = new QAction("Open",this);
	quitAppAction = new QAction("Quit",this);

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
		QString fileName = QFileDialog::getOpenFileName(this,"Open",0,tr("*.tds"));
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
