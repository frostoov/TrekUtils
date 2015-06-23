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
#include "json.hpp"

class MainWindow : public QMainWindow {
	Q_OBJECT
  public:
	explicit MainWindow(const trek::ChamberConfig& chamberConfig,	QWidget* parent = nullptr);
	~MainWindow();
  protected:
	virtual void keyPressEvent(QKeyEvent* ke);

	void createAction();
	void createMenues();
	void errorMessage(const QString& title, const QString& message);
  protected slots:
	void openData();
  private:
	trek::TrekHandler* mTrekHandler;
	TrekGLWidget*      mTrekWidget;
	tdcdata::DataSet   mBuffer;

	QAction* openEvtAction;
	QAction* quitAppAction;
	QMenu*   fileMenu;

	size_t mCurrentEvent;
};

#endif // MAINWINDOW_HPP
