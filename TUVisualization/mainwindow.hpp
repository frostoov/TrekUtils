#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QKeyEvent>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>

#include "tdcdata/dataset.hpp"
#include "trek/chamberrender.hpp"

#include "appconfigparser.hpp"
#include "configparser/chamberconfigparser.hpp"

class MainWindow : public QMainWindow {
	using Line2         = vecmath::Line2;
	using ChamberConfig = trek::ChamberConfig;
	using ChamberEventHandler = trek::ChamberHandler;
	using TUDataSet = tdcdata::DataSet;
	Q_OBJECT
  public:
	explicit MainWindow(QWidget* parent = nullptr);
  protected:
	void createWidgets();
	void packWidgets();

	void loadEvent();
	void openFile();
	void setCurrentEvent();
	void setCurrentChamber();
	void createAction();
	void createMenu();
	void errorMessage(const QString& title, const QString& message);
	void setPixmap();

	virtual void keyPressEvent(QKeyEvent* evt);
	virtual void resizeEvent(QResizeEvent* evt);
  private:
	QWidget*     centralWidget;
	QLabel*      imageLabel;
	QMenu*		 menu;
	QAction*	 openAction;
	QLineEdit*	 currentEventL;
	QListWidget* chamberList;

	TUDataSet			mEventBuffer;
	ChamberEventHandler mChamberHandler;
	ChamberRender		mChamberRender;
	Line2				uraganProjection;
	size_t				mCurrentEvent;
	size_t				mCurrentChamber;

	ChamberConfig mChamberConfig;
	AppConfig	  mAppConfig;
};

#endif // MAINWINDOW_HPP
