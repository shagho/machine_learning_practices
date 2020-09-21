TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        data.cpp \
        main.cpp

HEADERS += \
    data.h \
    plot.h \
    plot.h \
    utils.h \
    utils.h

unix|win32: LIBS += -lshogun

unix|win32: LIBS += -lcblas
