#
# Copyright (C) 2018 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include('./../QuasarAppLib/deploy.pri');
include('./../QuasarAppLib/QuasarLib.pri');

TARGET = CQtDeployer

SOURCES += \
        main.cpp \
    deploy.cpp

HEADERS += \
    deploy.h

QMAKE_LFLAGS += -Wl,-rpath,"'$$DESTDIR'"

DISTFILES += \
    ../snapBuild.sh \
    ../staticBuild.sh \
    ../snap/snapcraft.yaml \
    ../README.md \
    ../sharedBuild.sh
