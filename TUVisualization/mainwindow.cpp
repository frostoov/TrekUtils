#include <iostream>
#include <stdexcept>

#include "appconfigparser.hpp"
#include "mainwindow.hpp"

using tdcdata::DataSet;
using tdcdata::TUEvent;
using std::exception;
using std::cout;
using std::endl;
using std::set;

MainWindow::MainWindow(QWidget* parent) :
	QMainWindow(parent), mCurrentEvent(0), mCurrentChamber(0) {
	mChamberRender.setManager(&mChamberHandler);
	createWidgets();
	packWidgets();
	createAction();
	createMenu();

	connect(openAction,		&QAction::triggered,
			this,			&MainWindow::openFile);
	connect(currentEventL,	&QLineEdit::editingFinished,
			this,			&MainWindow::setCurrentEvent);
	connect(chamberList,	&QListWidget::currentRowChanged,
			this,			&MainWindow::setCurrentChamber);

	setCentralWidget(centralWidget);
	ChamberConfigParser chamberParser;
	try {
		chamberParser.load("chambers.conf");
		mChamberConfig = chamberParser.getConfig();
	} catch( const exception& e) {
		errorMessage("Exception", QString("Chambers Config: ") + e.what());
	}
	AppConfigParser appParser;
	try {
		appParser.load("TUVisualization.conf");
		mAppConfig     = appParser.getConfig();
		try {
			mChamberHandler.setSpeed(std::stod(mAppConfig.at("speed")));
			mChamberHandler.setPedestal(std::stoul(mAppConfig.at("pedestal")));
		} catch(const exception& e) {
			errorMessage("Exception", QString("TUVisualization Config: ") + e.what());
		}
	} catch(const exception& e) {
		errorMessage("Exception", QString("TUVisualization Config: ") + e.what());
	}

}

void MainWindow::createWidgets() {
	centralWidget	= new QWidget(this);
	imageLabel		= new QLabel(this);
	currentEventL	= new QLineEdit(this);
	chamberList		= new QListWidget(this);
}

void MainWindow::packWidgets() {
	auto mainLayout = new QHBoxLayout();
	auto subLayout	= new QVBoxLayout();

	mainLayout->addLayout(subLayout,1);
	mainLayout->addWidget(imageLabel,8);

	subLayout->addWidget(currentEventL);
	subLayout->addWidget(chamberList);
	centralWidget->setLayout(mainLayout);
}

void MainWindow::loadEvent() {
	try {
		auto  chamberEvent = mEventBuffer.at(mCurrentEvent).getChamberEvent(mCurrentChamber);
		auto& uraganEvent  = mEventBuffer.at(mCurrentEvent).getUraganEvent();
		mChamberHandler.setChamberData( chamberEvent );
		mChamberHandler.setUraganData( uraganEvent );
	} catch(const exception& e) {
		errorMessage("Exception", e.what());
	}
}

void MainWindow::openFile() {
	try {
		auto fileName = QFileDialog::getOpenFileName(this, "Open file", QDir::currentPath(),"*.tds");
		if(fileName.isEmpty())
			return;
		try {
			mEventBuffer.read(fileName.toStdString());
		} catch(const exception& e) {
			errorMessage("Exception", QString("Reading file: ") + e.what());
		}
		mCurrentEvent = 0, mCurrentChamber = 0;

		chamberList->clear();
		auto triggChambers = mEventBuffer.getTriggeredChambers();
		for(auto cham : triggChambers)
			chamberList->addItem(QString::number(cham + 1));
	} catch(const exception& e) {
		errorMessage("Exception", e.what());
	}
}

void MainWindow::setCurrentEvent() {
	auto val = currentEventL->text().toULong();
	if(val >= mEventBuffer.size())
		currentEventL->setText(QString::number(mCurrentEvent));
	else {
		mCurrentEvent = val;
		loadEvent();
		setPixmap();
	}
}

void MainWindow::setCurrentChamber() {
	if(chamberList->currentItem() == nullptr)
		return;
	try {
		mCurrentChamber = chamberList->currentItem()->text().toULong() - 1;
		mChamberHandler.setChamberPosition(mChamberConfig.at(mCurrentChamber));
		loadEvent();
		setPixmap();
		update();
		setFocus();
	} catch(const exception& e) {
		errorMessage("Exception", e.what());
	}
}

void MainWindow::createAction() {
	openAction = new QAction("Open File",this);
	openAction->setShortcuts(QKeySequence::Open);
}

void MainWindow::createMenu() {
	menu = new QMenu;
	menu = menuBar()->addMenu("File");
	menu->addAction(openAction);
}

void MainWindow::keyPressEvent(QKeyEvent* evt) {
	if(chamberList->currentItem() == nullptr)
		return;
	try {
		QMainWindow::keyPressEvent(evt);
		if(evt->key() == Qt::Key_PageDown) {
			while(mCurrentEvent) {
				--mCurrentEvent;
				loadEvent();
				if(mChamberHandler.hasChamberTrack()) {
					currentEventL->setText(QString::number(mCurrentEvent));
					setPixmap();
					break;
				}
			}
		} else if(evt->key() == Qt::Key_PageUp) {
			while( mCurrentEvent + 1 < mEventBuffer.size()) {
				++mCurrentEvent;
				loadEvent();
				if(mChamberHandler.hasChamberTrack()) {
					currentEventL->setText(QString::number(mCurrentEvent));
					setPixmap();
					break;
				}
			}
		}
	} catch(const exception& e) {
		errorMessage("Exception", e.what());
	}
}

void MainWindow::resizeEvent(QResizeEvent* evt) {
	try {
		QMainWindow::resizeEvent(evt);
		setPixmap();
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

void MainWindow::setPixmap() {
	imageLabel->setPixmap( mChamberRender.getPixmap(imageLabel->width(), imageLabel->height()) );
	update();
}
