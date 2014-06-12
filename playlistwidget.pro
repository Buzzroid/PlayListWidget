######################################################################
# Automatically generated by qmake (2.01a) Di. Nov 17 01:18:39 2009
######################################################################

TEMPLATE = app
TARGET =
DEPENDPATH += . src src/playlistwidget src/playlistwidget/taglibinterface
INCLUDEPATH += . src src/playlistwidget src/playlistwidget/taglibinterface

# Input
HEADERS += src/mainwindow.h \
           src/playlistwidget/playlistwidgetnew.h \
           src/playlistwidget/taglibinterface/globals.h \
           src/playlistwidget/taglibinterface/mediafile.h \
           src/playlistwidget/taglibinterface/taginformation.h \
           src/playlistwidget/taglibinterface/taglibinterface.h
SOURCES += src/main.cpp \
           src/mainwindow.cpp \
           src/playlistwidget/playlistwidgetnew.cpp \
           src/playlistwidget/taglibinterface/mediafile.cpp \
           src/playlistwidget/taglibinterface/taglibinterface.cpp
RESOURCES += src/application.qrc


INCLUDEPATH		+=	. \
					$(HOME)/AudioLibs/include \
					$(HOME)/AudioLibs/include/taglib

LIBS += -L$(HOME)/AudioLibs/lib -ltag