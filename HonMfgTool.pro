#-------------------------------------------------
#
# Project created by QtCreator 2016-10-14T09:44:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SFastboot
TEMPLATE = app

RC_ICONS = icon/SFastboot.ico

SOURCES += main.cpp\
        mainwindow.cpp \
    filevariable.cpp \
    fastboot.cpp \
    updatethread.cpp

HEADERS  += mainwindow.h \
    filevariable.h \
    fastboot.h \
    updatethread.h

FORMS    += mainwindow.ui

DISTFILES +=

RESOURCES += \
    resource.qrc
