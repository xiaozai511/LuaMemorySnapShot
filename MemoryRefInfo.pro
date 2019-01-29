#-------------------------------------------------
#
# Project created by QtCreator 2019-01-22T10:48:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MemoryRefInfo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
CONFIG(debug, debug|release) : CONFIG += console

QMAKE_CXXFLAGS += -std=c++11


SOURCES += \
        main.cpp \
    mainwindow.cpp \
    treeDock.cpp \
    mytreeview.cpp \
    common.cpp \
    comparedock.cpp \
    refrencetreedock.cpp \
    memoryrefrenceinfo.cpp \
    parsefiledialog.cpp \
    finddock.cpp

HEADERS += \
        mainwindow.h \
    treeDock.h \
    mytreeview.h \
    common.h \
    common.h \
    comparedock.h \
    refrencetreedock.h \
    memoryrefrenceinfo.h \
    parsefiledialog.h \
    finddock.h

FORMS += \
        mainwindow.ui \
    comparedock.ui \
    refrencetreedock.ui \
    parsefiledialog.ui \
    finddock.ui
