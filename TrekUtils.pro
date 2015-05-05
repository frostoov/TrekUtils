TEMPLATE = subdirs

SUBDIRS += common
CONFIG  += oredered
CONFIG  -= debug_and_release

SUBDIRS += \
	TUDataSet \
#	TUVisualization \
	TrekViewer
