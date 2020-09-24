TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

unix|win32: LIBS += -ldlib

unix|win32: LIBS += -lstdc++fs

unix|win32: LIBS += -lcblas

unix|win32: LIBS += -llapack

HEADERS += \
    plot.h
