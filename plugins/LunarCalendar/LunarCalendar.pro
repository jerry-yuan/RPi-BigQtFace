#-------------------------------------------------
#
# Project created by QtCreator 2018-01-03T15:27:20
#
#-------------------------------------------------

QT       += widgets

TARGET = LunarCalendar
TEMPLATE = lib

DEFINES += LUNARCALENDAR_LIBRARY

SOURCES += \
    LCPanel.cpp \
    LunarCalendarInfo.cpp \
    LunarCalendarItem.cpp \
    LunarCalendarWidget.cpp

HEADERS += \
    LCPanel.h \
    LunarCalendarInfo.h \
    LunarCalendarItem.h \
    LunarCalendarWidget.h

unix {
    target.path = /home/pi/inject/plugins/
    INSTALLS += target
}

FORMS += \
    LCPanel.ui

RESOURCES += \
    resource.qrc
