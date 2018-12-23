#
# Copyright (C) 2018 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += QuasarAppLib \
    CQtDeployer

contains(DEFINES, WITH_TESTS) {
    SUBDIRS += \
    tests/TestOnlyC \
    tests/TestQtWidgets \
    tests/TestQMLWidgets
}

CQtDeployer.depends=QuasarAppLib

QuasarAppLib.file = $$PWD/QuasarAppLib/QuasarApp.pro
