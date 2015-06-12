TEMPLATE = app
TARGET   = TUDataSet
CONFIG   = exceptions c++11 warn_on
CONFIG   -= debug_and_release

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

LIBS += -lboost_filesystem -lboost_system
INCLUDEPATH += ../common

HEADERS += \
	tools.hpp \
	eventhandler/listinghandler.hpp \
	eventhandler/trackshandler.hpp \
	eventhandler/matrixhandler.hpp \
	eventhandler/parametershandler.hpp

SOURCES += \
	main.cpp \
	tools.cpp \
	eventhandler/listinghandler.cpp \
	eventhandler/trackshandler.cpp \
	eventhandler/matrixhandler.cpp \
	eventhandler/parametershandler.cpp

OBJECTS += \
	../common/flagparser.o \
	../common/chamberconfigparser.o \
	../common/appconfigparser.o \
	../common/chamberhandler.o \
	../common/dataset.o \
	../common/event.o \
	../common/chamber.o \
	../common/trekhandler.o \

