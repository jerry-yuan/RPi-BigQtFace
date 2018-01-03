#-------------------------------------------------
#
# Project created by QtCreator 2018-01-03T15:19:42
#
#-------------------------------------------------

QT       += widgets

TARGET = PluginTemplate
TEMPLATE = lib

DEFINES += PLUGINTEMPLATE_LIBRARY

SOURCES += \
    PluginTemplate.cpp

HEADERS += \
    PluginTemplate.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    PluginTemplate.ui
