#
# Copyright (C) 2018-2019 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#


#-------------------------------------------------
#
# Project created by QtCreator 2019-01-26T13:55:47
#
#-------------------------------------------------

QT       -= gui
CONFIG += c++17
TARGET = Deploy
TEMPLATE = lib

DEFINES += DEPLOY_LIBRARY

VERSION = 1.3.0.0

DEFINES += APP_VERSION='\\"$$VERSION\\"'

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG(release, debug|release): {
    DESTDIR="$$PWD/build/release"
} else {
    DESTDIR="$$PWD/build/debug"
}

include('$$PWD/../QuasarAppLib/QuasarLib.pri')
include('$$PWD/../pe/pe-parser-library/pe-parser-library.pri')


SOURCES += \
    distrostruct.cpp \
    configparser.cpp \
    deploy.cpp \
    deploycore.cpp \
    envirement.cpp \
    extracter.cpp \
    filemanager.cpp \
    ignorerule.cpp \
    metafilemanager.cpp \
    pe.cpp \
    igetlibinfo.cpp \
    dependenciesscanner.cpp \
    ../qtTools/src/shared/winutils/elfreader.cpp \
    elf.cpp \
    pluginsparser.cpp \
    qml.cpp \
    libinfo.cpp

HEADERS += \
    distrostruct.h \
    configparser.h \
    deploy.h \
    deploy_global.h \
    deploycore.h \
    envirement.h \
    extracter.h \
    filemanager.h \
    ignorerule.h \
    metafilemanager.h \
    pe.h \
    igetlibinfo.h \
    dependenciesscanner.h \
    ../qtTools/src/shared/winutils/elfreader.h \
    elf.h \
    pluginsparser.h \
    qml.h \
    libinfo.h

STATECHARTS +=
