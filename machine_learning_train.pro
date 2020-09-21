TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        data.cpp \
        main.cpp

HEADERS += \
    data.h \
    plot.h \
    utils.h

unix|win32: LIBS += -lshogun

unix|win32: LIBS += -lcblas
