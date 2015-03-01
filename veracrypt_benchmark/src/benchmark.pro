#-------------------------------------------------
#
# Project created by QtCreator 2015-03-01T15:02:24
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = benchmark
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    benchmark.cpp

HEADERS += \
    benchmark.h task.h
LIBS += -lcryptsetup
