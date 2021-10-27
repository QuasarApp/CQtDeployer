#
# Copyright (C) 2018-2021 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

TEMPLATE = subdirs
CONFIG += ordered

lessThan(QT_MAJOR_VERSION, 6):lessThan(QT_MINOR_VERSION, 12) {
    warning("Tests are only enabled on Qt 5.12.0 or later version. You are using $$[QT_VERSION].")
    DEFINES += WITHOUT_TESTS
    DEFINES += WITHOUT_TR
}

android: DEFINES += WITHOUT_TESTS

!android {
    SUBDIRS += QuasarAppLib \
               Pe \
               zip \
               Deploy \
               CQtDeployer \
               UnitTests \
               tests/TestOnlyC \
               tests/TestCPPOnly \
               tests/TestQtWidgets \
               tests/TestQMLWidgets \
               tests/virtualkeyboard

    unix:SUBDIRS += tests/quicknanobrowser
    unix:SUBDIRS += tests/webui


    contains(DEFINES, WITHOUT_TESTS) {
        SUBDIRS -= UnitTests \
               tests/TestOnlyC \
               tests/TestCPPOnly \
               tests/TestQtWidgets \
               tests/TestQMLWidgets \
               tests/quicknanobrowser \
               tests/webui
    }

    CQtDeployer.depends=QuasarAppLib
    CQtDeployer.depends=Deploy

    QuasarAppLib.file = $$PWD/QuasarAppLib/QuasarApp.pro
    Pe.file = $$PWD/pe/pe-parser-library/pe-parser-library.pro

    include('$$PWD/QIFData/installerCQtDeployer.pri')
    !contains(QMAKE_HOST.arch, arm.*):{
        include($$PWD/test.pri)
    }

    DISTFILES += \
        snap/snapcraft.yaml \
        snap/gui/cqtdeployer.desktop \
        README.md \


}

