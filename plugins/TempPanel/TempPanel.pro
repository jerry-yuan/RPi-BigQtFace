#-------------------------------------------------
#
# Project created by QtCreator 2017-07-27T22:35:58
#
#-------------------------------------------------

QT       += widgets printsupport

QT       -= gui

TARGET = TempPanel
TEMPLATE = lib

DEFINES += TEMPPANEL_LIBRARY

SOURCES += \
    qcustomplot.cpp \
    TempPanel.cpp

HEADERS += \
    qcustomplot.h \
    TempPanel.h

unix {
    target.path = /home/pi/inject/plugins/
    INSTALLS += target
}

FORMS += \
    TempPanel.ui

RESOURCES += \
    Icons.qrc
