#-------------------------------------------------
#
# Project created by QtCreator 2017-06-25T17:32:59
#
#-------------------------------------------------

QT       += core gui sql network websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BigFace
TEMPLATE = app
DEFINES += QT_MESSAGELOGCONTEXT

SOURCES += main.cpp\
        Face.cpp \
    NetworkMonitor.cpp \
    weather/WeatherWidget.cpp \
    weather/WindDirectDisplay.cpp \
    NetworkSpeed.cpp \
    Clock.cpp \
    SysFuncDialog.cpp \
    HaltDialog.cpp \
    EventServer.cpp \
    SysResMonitor.cpp \
    WebsocketServer.cpp \
    Logger.cpp \
    BeeperThread.cpp

HEADERS  += Face.h \
    NetworkMonitor.h \
    weather/WeatherWidget.h \
    weather/WindDirectDisplay.h \
    Consts.h \
    NetworkSpeed.h \
    Clock.h \
    SysFuncDialog.h \
    HaltDialog.h \
    EventServer.h \
    FuncPanel.h \
    SysResMonitor.h \
    WebsocketServer.h \
    Logger.h \
    BeeperThread.h

FORMS    += Face.ui \
    NetworkMonitor.ui \
    weather/WeatherWidget.ui \
    NetworkSpeed.ui \
    SysFuncDialog.ui \
    HaltDialog.ui \
    SysResMonitor.ui

target.path=/home/pi/inject
INSTALLS+=target

RESOURCES += \
    weather/weatherIcon.qrc \
    icons.qrc
unix{
    LIBS+= -lwiringpi
}
