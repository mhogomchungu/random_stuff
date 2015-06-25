#-------------------------------------------------
#
# Project created by QtCreator 2015-05-29T14:18:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = flashDownloader
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

RESOURCES += icons.qrc

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h task.h

FORMS    += mainwindow.ui
