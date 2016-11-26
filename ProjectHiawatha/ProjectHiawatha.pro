#-------------------------------------------------
#
# Project created by QtCreator 2016-09-10T18:52:33
#
#-------------------------------------------------

QT       += core gui
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
    player.cpp \
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
    citylabel.cpp \
    heap.cpp \
    about.cpp \
    notificationsystem.cpp

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
    player.h \
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
    citylabel.h \
    unittype.h \
    heap.h \
    tiledata.h \
    datatypes.h \
    resources.h \
    about.h \
    notificationsystem.h

FORMS    += mainwindow.ui \
    gameoptions.ui \
    cityscreen.ui \
    techtree.ui \
    about.ui
