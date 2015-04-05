QT = core gui widgets

TARGET = TrekViewer
TEMPLATE = app
CONFIG += c++11

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

INCLUDEPATH += ../common

OBJECTS += \
	../common/appconfigparser.o \
	../common/chamberconfigparser.o \
	../common/chamberhandler.o \
	../common/chamberrender.o \
	../common/dataset.o \
	../common/event.o \
	../common/eventhandler.o \
	../common/paramshandler.o \
	../common/chamber.o \
	../common/trekhandler.o \

HEADERS += \
    mainwindow.hpp \
    trackglwidget.hpp

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    trackglwidget.cpp
