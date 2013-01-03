#-------------------------------------------------
#
# Project created by QtCreator 2012-11-10T18:22:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


VERSION = 1.0.0

QMAKE_CXXFLAGS_RELEASE += -O4
QMAKE_CFLAGS_RELEASE += -O4

TARGET = Mousoid
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.hpp

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../MousoidCore_build/release -lMousoidCore1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../MousoidCore_build/debug -lMousoidCored1
else:unix:!macx: LIBS += -L$$PWD/../MousoidCore_build_Debug/ -lMousoidCore

INCLUDEPATH += $$PWD/../MousoidCore
DEPENDPATH += $$PWD/../MousoidCore

OTHER_FILES += \
    icon.png \
    912.ico \
    899.ico \
    886.ico

RESOURCES += \
    resources.qrc
