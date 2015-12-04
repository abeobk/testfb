QT += core
QT -= gui

TARGET = fbtest
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    abeofbdirectdraw.cpp \
    abeoserialport.cpp \
    abeotouchscreen.cpp


target.files=fbtest
target.path=/root
INSTALLS = target

LIBS += -L~/qt/tslib/lib/libts-1.0.so.0
#QMAKE_LFLAGS += -Wl, -rpath=~/qt/tslib/lib

HEADERS += \
    abeofbdirectdraw.h \
    type.h \
    error.h \
    rasterfont.h \
    abeoserialport.h \
    abeotouchscreen.h
