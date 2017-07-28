#-------------------------------------------------
#
# Project created by QtCreator 2017-06-25T17:32:59
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BigFace
TEMPLATE = app


SOURCES += main.cpp\
        Face.cpp \
    NetworkMonitor.cpp \
    weather/WeatherWidget.cpp \
    weather/WindDirectDisplay.cpp \
    Logger.cpp \
    DBUtil.cpp \
    NetworkSpeed.cpp \
    Clock.cpp \
    SysFuncDialog.cpp \
    HaltDialog.cpp \
    EventServer.cpp \
    GPIOAdapter.cpp

HEADERS  += Face.h \
    NetworkMonitor.h \
    weather/WeatherWidget.h \
    weather/WindDirectDisplay.h \
    Logger.h \
    DBUtil.h \
    Consts.h \
    NetworkSpeed.h \
    Clock.h \
    SysFuncDialog.h \
    HaltDialog.h \
    EventServer.h \
    GPIOAdapter.h \
    FuncPanel.h

FORMS    += Face.ui \
    NetworkMonitor.ui \
    weather/WeatherWidget.ui \
    NetworkSpeed.ui \
    SysFuncDialog.ui \
    HaltDialog.ui \
    Temperature.ui

target.path=/home/pi/inject
INSTALLS+=target

RESOURCES += \
    weather/weatherIcon.qrc \
    icons.qrc
