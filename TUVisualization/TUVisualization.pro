QT = core gui widgets

TARGET   = TUVisualization
TEMPLATE = app
CONFIG   += exceptions c++11 warn_on

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

HEADERS += \
	appconfigparser.hpp \
	mainwindow.hpp

SOURCES += \
	appconfigparser.cpp \
	main.cpp \
	mainwindow.cpp

INCLUDEPATH += ../common
OBJECTS += \
	../common/chamberconfigparser.o \
	../common/chamberhandler.o \
	../common/chamberrender.o \
	../common/dataset.o \
	../common/event.o \
	../common/eventhandler.o \
	../common/paramshandler.o \
