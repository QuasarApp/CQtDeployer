#
# Copyright (C) 2018-2021 QuasarApp.
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

VERSION = 1.5.0.25

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
include('$$PWD/../zip/zip.pri')
include('$$PWD/../LIEF/includeLIEF.pri')

INCLUDEPATH += $$PWD/Parsers


SOURCES += \
    Distributions/deb.cpp \
    Distributions/defaultdistro.cpp \
    Distributions/templateinfo.cpp \
    Distributions/ziparhive.cpp \
    Parsers/macho_type.cpp \
    dependencymap.cpp \
    deployconfig.cpp \
    distromodule.cpp \
    distrostruct.cpp \
    configparser.cpp \
    deploy.cpp \
    deploycore.cpp \
    Parsers/elf_type.cpp \
    envirement.cpp \
    extra.cpp \
    extracter.cpp \
    filemanager.cpp \
    Distributions/idistribution.cpp \
    Parsers/generalfiles_type.cpp \
    ignorerule.cpp \
    metafilemanager.cpp \
    packagecontrol.cpp \
    packing.cpp \
    pathutils.cpp \
    igetlibinfo.cpp \
    dependenciesscanner.cpp \
    ../qtTools/src/shared/winutils/elfreader.cpp \
    Parsers/pe_type.cpp \
    pluginsparser.cpp \
    Distributions/qif.cpp \
    qml.cpp \
    libinfo.cpp \
    qtdir.cpp \
    targetdata.cpp \
    targetinfo.cpp \
    zipcompresser.cpp

HEADERS += \
    Distributions/deb.h \
    Distributions/defaultdistro.h \
    Distributions/templateinfo.h \
    Distributions/ziparhive.h \
    Parsers/macho_type.h \
    defines.h \
    dependencymap.h \
    deployconfig.h \
    distromodule.h \
    distrostruct.h \
    configparser.h \
    deploy.h \
    deploy_global.h \
    deploycore.h \
    Parsers/elf_type.h \
    envirement.h \
    extra.h \
    extracter.h \
    filemanager.h \
    Distributions/idistribution.h \
    Parsers/generalfiles_type.h \
    ignorerule.h \
    metafilemanager.h \
    packagecontrol.h \
    packing.h \
    pathutils.h \
    igetlibinfo.h \
    dependenciesscanner.h \
    ../qtTools/src/shared/winutils/elfreader.h \
    Parsers/pe_type.h \
    pluginsparser.h \
    Distributions/qif.h \
    qml.h \
    libinfo.h \
    qtdir.h \
    targetdata.h \
    targetinfo.h \
    zipcompresser.h


STATECHARTS +=

RESOURCES += \
    DeployResources.qrc
