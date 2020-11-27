#
# Copyright (C) 2018-2020 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

TEMPLATE = subdirs
CONFIG += ordered

lessThan(QT_MAJOR_VERSION, 6):lessThan(QT_MINOR_VERSION, 14) {
    message(Tests is disabled!)
    DEFINES += WITHOUT_TESTS
}

!android {
    SUBDIRS += QuasarAppLib \
               Pe \
               Deploy \
               CQtDeployer \
               UnitTests \
               tests/TestOnlyC \
               tests/TestQtWidgets \
               tests/TestQMLWidgets

    unix:SUBDIRS += tests/quicknanobrowser
    unix:SUBDIRS += tests/webui


    contains(DEFINES, WITHOUT_TESTS) {
        SUBDIRS -= UnitTests \
               tests/TestOnlyC \
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
    include($$PWD/doc/wiki.pri)

    DISTFILES += \
        snap/snapcraft.yaml \
        snap/gui/cqtdeployer.desktop \
        README.md \


}
    include($$PWD/test.pri)

