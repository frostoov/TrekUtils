TEMPLATE = app
TARGET   = TUDataSet
CONFIG   = exceptions c++11 warn_on
CONFIG -= debug_and_release

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

INCLUDEPATH += ../common

HEADERS += \
	tools.hpp \
	tracktest.hpp

SOURCES += \
	main.cpp \
	tools.cpp \
	tracktest.cpp

OBJECTS += \
	../common/flagparser.o \
	../common/chamberconfigparser.o \
	../common/chamberhandler.o \
	../common/dataset.o \
	../common/event.o \
	../common/eventhandler.o \
	../common/paramshandler.o \
	../common/chamber.o \
	../common/trekhandler.o \

