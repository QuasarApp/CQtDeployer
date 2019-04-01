TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

DESTDIR="$$PWD/../build"

RESOURCES += \
    conf.qrc
