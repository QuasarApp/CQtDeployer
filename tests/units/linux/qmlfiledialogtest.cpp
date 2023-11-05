//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "qmlfiledialogtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>
#include "modules.h"


void QmlFileDialogTest::test() {
    
    TestUtils utils;

    auto compareTree = TestModule.qmlLibs();

    compareTree = TestModule.ignoreFilter(compareTree, "TestQMLWidgets");

    compareTree += utils.createTree(
        {
            "./" + DISTRO_DIR + "/QMLFileDialog.sh",
            "./" + DISTRO_DIR + "/bin/QMLFileDialog",
            "./" + DISTRO_DIR + "/lib/libQt6QmlCore.so",
            "./" + DISTRO_DIR + "/lib/libQt6QuickDialogs2.so",
            "./" + DISTRO_DIR + "/lib/libQt6QuickDialogs2QuickImpl.so",
            "./" + DISTRO_DIR + "/lib/libQt6QuickDialogs2Utils.so",
            "./" + DISTRO_DIR + "/qml/QtCore/libqtqmlcoreplugin.so",
            "./" + DISTRO_DIR + "/qml/QtCore/plugins.qmltypes",
            "./" + DISTRO_DIR + "/qml/QtCore/qmldir",
            "./" + DISTRO_DIR + "/qml/QtQuick/Dialogs/libqtquickdialogsplugin.so",
            "./" + DISTRO_DIR + "/qml/QtQuick/Dialogs/plugins.qmltypes",
            "./" + DISTRO_DIR + "/qml/QtQuick/Dialogs/qmldir",
        }
        );

#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    compareTree += utils.createTree(
        {
            "./" + DISTRO_DIR + "/lib/libQt6QuickLayouts.so",
            "./" + DISTRO_DIR + "/qml/QtQuick/Dialogs/quickimpl/libqtquickdialogs2quickimplplugin.so",
            "./" + DISTRO_DIR + "/qml/QtQuick/Dialogs/quickimpl/plugins.qmltypes",
            "./" + DISTRO_DIR + "/qml/QtQuick/Dialogs/quickimpl/qmldir",
            "./" + DISTRO_DIR + "/qml/QtQuick/Layouts/libqquicklayoutsplugin.so",
            "./" + DISTRO_DIR + "/qml/QtQuick/Layouts/plugins.qmltypes",
            "./" + DISTRO_DIR + "/qml/QtQuick/Layouts/qmldir"

        }
        );
#endif

    auto bin = TestBinDir + "QMLFileDialog";

    runTestParams({"-bin", bin, "clear" ,
                   "-qmlDir", TestBinDir + "/../QMLFileDialog"}, &compareTree);

}

