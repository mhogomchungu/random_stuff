#-------------------------------------------------
#
# Project created by QtCreator 2014-12-27T14:40:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fileServer
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp fileserver.cpp statusicon.cpp mainwindow.cpp

HEADERS  += fileserver.h statusicon.h mainwindow.h \
    task.h

FORMS    += mainwindow.ui

LIBS += -lkdecore -lkdeui

RESOURCES += \
    icons.qrc

FORMS +=
