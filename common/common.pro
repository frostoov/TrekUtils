TEMPLATE = lib
CONFIG  += qt exceptions c++11
CONFIG  -= debug_and_release

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

HEADERS += \
	configparser/appconfigparser.hpp \
	configparser/chamberconfigparser.hpp \
	configparser/configparser.hpp \
	math/coordsystem.hpp \
	math/figures.hpp \
	math/histogram.hpp \
	math/lines.hpp \
	math/matrix.hpp \
	math/vec.hpp \
	tdcdata/dataset.hpp \
	tdcdata/event.hpp \
	tdcdata/structs.hpp \
	trek/chamber.hpp \
	trek/chamberhandler.hpp \
	trek/trekhandler.hpp \
	serialization.hpp \
	configparser/flagparser.hpp \
    json.hpp

SOURCES += \
	configparser/appconfigparser.cpp \
	configparser/chamberconfigparser.cpp \
	tdcdata/dataset.cpp \
	tdcdata/event.cpp \
	trek/chamber.cpp \
	trek/chamberhandler.cpp \
	trek/trekhandler.cpp \
	configparser/flagparser.cpp
