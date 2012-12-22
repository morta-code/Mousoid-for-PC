#-------------------------------------------------
#
# Project created by QtCreator 2012-11-10T18:22:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mousoid
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.hpp

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../MousoidCore_build_Debug -lMousoidCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../MousoidCore_build_Debug/ -lMousoidCored
else:unix:!macx: LIBS += -L$$PWD/../MousoidCore_build_Debug/ -lMousoidCore

INCLUDEPATH += $$PWD/../MousoidCore
DEPENDPATH += $$PWD/../MousoidCore
