#
# Copyright (C) 2018-2019 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

QT -= gui

CONFIG += c++14 console
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

CONFIG(release, debug|release): {
    DESTDIR="$$PWD/../build/release"
} else {
    DESTDIR="$$PWD/../build/debug"
}

isEmpty(PREFIX){
    PREFIX=$$PWD/../distro
}
include('$$PWD/../QuasarAppLib/QuasarLib.pri')
include('$$PWD/../QuasarAppLib/Etalons/qmake/install_prefix.pri')

install_data.files += $$DESTDIR/$$runfiletype
install_data.files += $$QUASARAPP_LIB_OUTPUT_DIR/$$libfiletype


TARGET = cqtdeployer

SOURCES += \
        main.cpp \
        deploy.cpp \
        deployutils.cpp \
        windependenciesscanner.cpp \
        ../qtTools/src/windeployqt/elfreader.cpp \
        ../qtTools/src/windeployqt/utils.cpp

HEADERS += \
        deploy.h \
        deployutils.h \
        windependenciesscanner.h \
        ../qtTools/src/windeployqt/elfreader.h \
        ../qtTools/src/windeployqt/utils.h


DISTFILES += \
    ../snapBuild.sh \
    ../staticBuild.sh \
    ../snap/snapcraft.yaml \
    ../README.md \
    ../sharedBuild.sh \
    ../README.md \
    ../staticBuildCrossWin.sh \
    ../sharedBuild.bat

win32: LIBS += -lshlwapi
win32: RC_ICONS = $$PWD/../res/icon.ico
