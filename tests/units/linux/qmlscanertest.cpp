//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "qmlscanertest.h"
#include "qmlqt6.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void QmlScanerTest::test() {
    // qt5
    auto qmlRoot = QFileInfo(TestQtDir + "/qml").absoluteFilePath();
    QMLQt6 scaner6(qmlRoot);

    // qt6

    QSet<QString> results = {
        {qmlRoot + "/QtQuick"},
        {qmlRoot + "/QtQuick/Controls"},
        {qmlRoot + "/QtQuick/Controls/Material"},
        {qmlRoot + "/QtQuick/Layouts"},
    };

    auto imports = scaner6.extractImportsFromFile(":/qmlFileQt6.qml");

    QVERIFY(results.size() == imports.size());

    for (const auto &import: std::as_const(imports)) {
        auto path = scaner6.getPathFromImport(import);
        QVERIFY(results.contains(path));
    }

    imports = scaner6.extractImportsFromFile(":/qmlFile.qml");

    QVERIFY(results.size() == imports.size());

    for (const auto & import: std::as_const(imports)) {
        auto path = scaner6.getPathFromImport(import);
        QVERIFY(results.contains(path));
    }

    // qt6

    results = {
        {qmlRoot + "/QtQuick"},
        {qmlRoot + "/QtQuick/Window"},
        {qmlRoot + "/QtQuick/Layouts"},
        {qmlRoot + "/Qt/labs/folderlistmodel"},
        {qmlRoot + "/QtQuick/VirtualKeyboard/Settings"},
        {qmlRoot + "/QtQuick/VirtualKeyboard/Styles"},
    };

    imports = scaner6.extractImportsFromQmlModule(":/qmlDir");

    QVERIFY(results.size() == imports.size());

    for (const auto &import: std::as_const(imports)) {
        auto path = scaner6.getPathFromImport(import);
        QVERIFY(results.contains(path));
    }

}
