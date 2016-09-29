#-------------------------------------------------
#
# Project created by QtCreator 2016-09-10T18:52:33
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NotCiv
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tile.cpp \
    yield.cpp \
    renderer.cpp \
    map.cpp \
    gameview.cpp \
    ai_tactical.cpp \
    ai_strategic.cpp \
    gamemanager.cpp \
    gameoptions.cpp

HEADERS  += mainwindow.h \
    tile.h \
    tiletype.h \
    yield.h \
    renderer.h \
    tileimprovement.h \
    map.h \
    gameview.h \
    ai_tactical.h \
    ai_strategic.h \
    biome.h \
    gamemanager.h \
    gameoptions.h

FORMS    += mainwindow.ui \
    gameoptions.ui
