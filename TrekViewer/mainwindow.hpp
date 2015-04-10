#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QApplication>
#include <QMainWindow>
#include <QFileDialog>
#include <QKeyEvent>
#include <QAction>
#include <QMenuBar>
#include <QMenu>

#include "trek/trekhandler.hpp"
#include "tdcdata/dataset.hpp"
#include "configparser/chamberconfigparser.hpp"
#include "configparser/appconfigparser.hpp"
#include "trackglwidget.hpp"

class MainWindow : public QMainWindow {
	using DataSet       = tdcdata::DataSet;
	using ChamberConfig = trek::ChamberConfig;
	using TrekHandler   = trek::TrekHandler;
	Q_OBJECT
  public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();
  protected:
	virtual void keyPressEvent(QKeyEvent* ke);

	void createAction();
	void createMenues();
	void errorMessage(const QString& title, const QString& message);
  protected slots:
	void openData();
  private:
	TrekHandler*   mTrekHandler;
	TrekGLWidget*  mTrekWidget;
	DataSet		  mBuffer;

	QAction* openEvtAction;
	QAction* quitAppAction;
	QMenu*   fileMenu;

	ChamberConfig mChamberConfig;
	AppConfig	  mAppConfig;

	size_t mCurrentEvent;
};

#endif // MAINWINDOW_HPP
