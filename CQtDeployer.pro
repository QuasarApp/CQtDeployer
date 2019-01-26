#
# Copyright (C) 2018-2019 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += QuasarAppLib \
           Deploy \
           CQtDeployer \
           UnitTests


contains(DEFINES, WITH_TESTS) {
    SUBDIRS += \
    tests/TestOnlyC \
    tests/TestQtWidgets \
    tests/TestQMLWidgets
}

CQtDeployer.depends=QuasarAppLib
CQtDeployer.depends=Deploy

QuasarAppLib.file = $$PWD/QuasarAppLib/QuasarApp.pro

win32:include('$$PWD/CQtDeployerWinBuild.pri')

DISTFILES += \
    snapBuild.sh \
    staticBuild.sh \
    snap/snapcraft.yaml \
    README.md \
    sharedBuild.sh \
    README.md \
    staticBuildCrossWin.sh \
    sharedBuild.bat
