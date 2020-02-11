#
# Copyright (C) 2018-2020 QuasarApp.
# Distributed under the lgplv3 software license, see the accompanying
# Everyone is permitted to copy and distribute verbatim copies
# of this license document, but changing it is not allowed.
#

TEMPLATE = subdirs
CONFIG += ordered
!android {
    SUBDIRS += QuasarAppLib \
               Pe \
               Deploy \
               CQtDeployer \
               UnitTests \
               tests/TestOnlyC \
               tests/TestQtWidgets \
               tests/TestQMLWidgets


    contains(DEFINES, WITH_ALL_TESTS) {
        SUBDIRS += tests/quicknanobrowser
    }


    contains(DEFINES, WITHOUT_TESTS) {
        SUBDIRS -= UnitTests \
               tests/TestOnlyC \
               tests/TestQtWidgets \
               tests/TestQMLWidgets \
               tests/quicknanobrowser
    }

    CQtDeployer.depends=QuasarAppLib
    CQtDeployer.depends=Deploy

    QuasarAppLib.file = $$PWD/QuasarAppLib/QuasarApp.pro
    Pe.file = $$PWD/pe/pe-parser-library/pe-parser-library.pro

    include('$$PWD/QIFData/installerCQtDeployer.pri')
    include($$PWD/doc/wiki.pri)

    DISTFILES += \
        snap/snapcraft.yaml \
        README.md \


}
    include($$PWD/test.pri)

