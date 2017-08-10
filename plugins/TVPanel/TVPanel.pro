#-------------------------------------------------
#
# Project created by QtCreator 2017-08-04T19:07:39
#
#-------------------------------------------------

QT       += widgets dbus

TARGET = TVPanel
TEMPLATE = lib

DEFINES += TVPANEL_LIBRARY

SOURCES += \
    TVPanel.cpp

HEADERS += \
    TVPanel.h

unix {
    target.path = /home/pi/inject/plugins/
    INSTALLS += target
}

FORMS += \
    TVPanel.ui

RESOURCES += \
    resources.qrc
