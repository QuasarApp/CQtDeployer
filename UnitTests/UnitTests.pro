#
# Copyright (C) 2018-2020 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#


QT += testlib
QT -= gui

CONFIG(release, debug|release): {
    DESTDIR="$$PWD/build/release"
} else {
    DESTDIR="$$PWD/build/debug"
}

include('$$PWD/../QuasarAppLib/QuasarLib.pri')
include('$$PWD/../Deploy/Deploy.pri')
include('$$PWD/../zip/zip.pri')

include('$$PWD/../pe/pe-parser-library/pe-parser-library.pri')


QT_DIR = $$[QT_HOST_BINS]/../
DEFINES+=QT_BASE_DIR='\\"$$QT_DIR\\"'
DEFINES+=TEST_BIN_DIR='\\"$$PWD/../tests/build/\\"'
#DEFINES+=QTEST_FUNCTION_TIMEOUT=900000

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_deploytest.cpp \
    libcreator.cpp \
    modules.cpp \
    modulesqt513.cpp \
    modulesqt514.cpp \
    modulesqt515.cpp \
    qmlcreator.cpp \
    testutils.cpp

RESOURCES += \
    res.qrc

HEADERS += \
    backward-cpp/backward.hpp \
    libcreator.h \
    modules.h \
    modulesqt513.h \
    modulesqt514.h \
    modulesqt515.h \
    qmlcreator.h \
    testutils.h
