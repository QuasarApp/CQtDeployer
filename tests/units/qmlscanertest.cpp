//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "qmlscanertest.h"
#include "qml.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void QmlScanerTest::test() {
    // qt5
    auto qmlRoot = QFileInfo(TestQtDir + "/qml").absoluteFilePath();
    QML *scaner = new QML(qmlRoot, QtMajorVersion::Qt5);
    auto imports = scaner->extractImportsFromFile(":/qmlFile.qml");
    scaner->scanQmlTree(qmlRoot);

        QSet<QString> results = {
                                 {qmlRoot + "/QtQuick.2/"},
                                 {qmlRoot + "/QtQuick/Controls.2/"},
                                 {qmlRoot + "/QtQuick/Controls.2/Material/"},
                                 {qmlRoot + "/QtQuick/Layouts/"},
                                 };

    QVERIFY(results.size() == imports.size());

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)

    for (const auto &import: qAsConst(imports)) {
        auto path = scaner->getPathFromImport(import);
        QVERIFY(results.contains(path));
    }
#endif

    // qt6
    scaner->setQtVersion(QtMajorVersion::Qt6);

    results = {
        {qmlRoot + "/QtQuick"},
        {qmlRoot + "/QtQuick/Controls"},
        {qmlRoot + "/QtQuick/Controls/Material"},
        {qmlRoot + "/QtQuick/Layouts"},
    };

    imports = scaner->extractImportsFromFile(":/qmlFileQt6.qml");

    QVERIFY(results.size() == imports.size());

    for (const auto &import: qAsConst(imports)) {
        auto path = scaner->getPathFromImport(import);
        QVERIFY(results.contains(path));
    }

    imports = scaner->extractImportsFromFile(":/qmlFile.qml");

    QVERIFY(results.size() == imports.size());

    for (const auto & import: qAsConst(imports)) {
        auto path = scaner->getPathFromImport(import);
        QVERIFY(results.contains(path));
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)

    // qt5
    scaner->setQtVersion(QtMajorVersion::Qt5);

    results = {
        {qmlRoot + "/QtQuick.2/"},
        {qmlRoot + "/QtQuick/Window.2/"},
        {qmlRoot + "/QtQuick/Layouts/"},
        {qmlRoot + "/Qt/labs/folderlistmodel/"},
        {qmlRoot + "/QtQuick/VirtualKeyboard/Settings/"},
        {qmlRoot + "/QtQuick/VirtualKeyboard/Styles/"},
    };

    imports = scaner->extractImportsFromQmlModule(":/qmlDir");

    QVERIFY(results.size() == imports.size());

    for (const auto &import: qAsConst(imports)) {
        auto path = scaner->getPathFromImport(import);
        QVERIFY(results.contains(path));
    }
#endif
    // qt6
    scaner->setQtVersion(QtMajorVersion::Qt6);

    results = {
        {qmlRoot + "/QtQuick"},
        {qmlRoot + "/QtQuick/Window"},
        {qmlRoot + "/QtQuick/Layouts"},
        {qmlRoot + "/Qt/labs/folderlistmodel"},
        {qmlRoot + "/QtQuick/VirtualKeyboard/Settings"},
        {qmlRoot + "/QtQuick/VirtualKeyboard/Styles"},
    };

    imports = scaner->extractImportsFromQmlModule(":/qmlDir");

    QVERIFY(results.size() == imports.size());

    for (const auto &import: qAsConst(imports)) {
        auto path = scaner->getPathFromImport(import);
        QVERIFY(results.contains(path));
    }

}
