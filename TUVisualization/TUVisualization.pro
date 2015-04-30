QT = core gui widgets

TARGET   = TUVisualization
TEMPLATE = app
CONFIG   += exceptions c++11 warn_on
CONFIG -= debug_and_release

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

HEADERS += \
	appconfigparser.hpp \
	mainwindow.hpp \
	chamberrender.hpp

SOURCES += \
	appconfigparser.cpp \
	main.cpp \
	mainwindow.cpp \
	chamberrender.cpp

INCLUDEPATH += ../common
OBJECTS += \
	../common/chamber.o \
	../common/chamberconfigparser.o \
	../common/chamberhandler.o \
	../common/dataset.o \
	../common/event.o \
