TEMPLATE = app
TARGET   = TUDataSet
CONFIG   = exceptions c++11 warn_on
CONFIG   -= debug_and_release

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

INCLUDEPATH += ../common

HEADERS += \
	tools.hpp \
	tracktest.hpp \
	eventhandler/paramshandler.hpp \
    eventhandler/listinghandler.hpp \
    eventhandler/trackshandler.hpp \
    eventhandler/matrixhandler.hpp

SOURCES += \
	main.cpp \
	tools.cpp \
	tracktest.cpp \
	eventhandler/paramshandler.cpp \
    eventhandler/listinghandler.cpp \
    eventhandler/trackshandler.cpp \
    eventhandler/matrixhandler.cpp

OBJECTS += \
	../common/flagparser.o \
	../common/chamberconfigparser.o \
	../common/chamberhandler.o \
	../common/dataset.o \
	../common/event.o \
	../common/chamber.o \
	../common/trekhandler.o \

