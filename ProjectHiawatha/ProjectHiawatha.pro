#-------------------------------------------------
#
# Project created by QtCreator 2016-09-10T18:52:33
#
#-------------------------------------------------

QT       += core gui concurrent
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

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
    gameoptions.cpp \
    civilization.cpp \
    ai_operational.cpp \
    cityscreen.cpp \
    gamescene.cpp \
    city.cpp \
    building.cpp \
    unit.cpp \
    unitcontroller.cpp \
    ai_controller.cpp \
    technology.cpp \
    techtree.cpp \
    heap.cpp \
    about.cpp \
    notificationsystem.cpp \
    diplomacy.cpp \
    savecontroller.cpp

    gameoptions.cpp \

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
    gameoptions.h \
    civilization.h \
    ai_operational.h \
    nation.h \
    cityscreen.h \
    civcolors.h \
    gamescene.h \
    city.h \
    building.h \
    unit.h \
    unitcontroller.h \
    ai_controller.h \
    technology.h \
    techtree.h \
    unittype.h \
    heap.h \
    datatypes.h \
    resources.h \
    about.h \
    notificationsystem.h \
    diplomacy.h \
    savecontroller.h

FORMS    += mainwindow.ui \
    gameoptions.ui \
    cityscreen.ui \
    techtree.ui \
    about.ui \
    diplomacy.ui
