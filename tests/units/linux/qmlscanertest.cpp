//#
//# Copyright (C) 2020-2026 QuasarApp.
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

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>


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

    // test for custom auto generated qml modules (created by qt_add_qml_module
    // without the NO_PLUGIN option). Such modules are built into the project
    // build dir and have to be resolved from the project dir, not from the
    // qt distribution.
    // see https://github.com/QuasarApp/CQtDeployer/issues/847
    {
        QTemporaryDir tmpDir;
        QVERIFY2(tmpDir.isValid(), "Failed to create temp dir for custom qml module test");

        const auto projectDir = QFileInfo(tmpDir.path()).absoluteFilePath();

        // create a custom qml module "cool_project" in the project dir
        const auto moduleDir = projectDir + "/cool_project";
        QVERIFY(QDir().mkpath(moduleDir));

        {
            QFile qmldir(moduleDir + "/qmldir");
            QVERIFY(qmldir.open(QFile::WriteOnly | QFile::Text));
            QTextStream stream(&qmldir);
            stream << "module cool_project\n"
                  << "Cool 1.0\n"
                  << "depends QtQuick auto\n";
            qmldir.close();
        }

        // a dummy plugin shared object of the custom module
        {
            QFile plugin(moduleDir + "/libcool_project.so");
            QVERIFY(plugin.open(QFile::WriteOnly));
            plugin.write("dummy-plugin");
            plugin.close();
        }

        // the application qml imports the custom module
        {
            QFile app(projectDir + "/app.qml");
            QVERIFY(app.open(QFile::WriteOnly | QFile::Text));
            QTextStream stream(&app);
            stream << "import cool_project\n";
            app.close();
        }

        QMLQt6 customScaner(qmlRoot);
        QStringList results;
        QVERIFY2(customScaner.scan(results, projectDir),
                 "Failed to scan custom qml module project dir");

        // the custom module is resolved to the project dir (not to the qt root)
        const auto expectedModule = QFileInfo(moduleDir).absoluteFilePath();
        QVERIFY2(results.contains(expectedModule),
                 ("Custom qml module is not resolved to the project dir: " +
                  expectedModule).toUtf8().constData());
        QCOMPARE(customScaner.getPathFromImport("cool_project"), expectedModule);

        // the qt module dependency of the custom module is still resolved to the qt root
        QVERIFY2(results.contains(qmlRoot + "/QtQuick"),
                 "QtQuick dependency of custom qml module is not resolved to the qt root");
    }

    // test for custom auto generated qml modules with a dotted uri, e.g.
    // "my.company.module". The import line stores the uri with "/" instead of
    // ".", so the module dir must be stored in the same format (see issue 847).
    {
        QTemporaryDir tmpDir;
        QVERIFY2(tmpDir.isValid(), "Failed to create temp dir for dotted qml module test");

        const auto projectDir = QFileInfo(tmpDir.path()).absoluteFilePath();

        const auto moduleDir = projectDir + "/build";
        QVERIFY(QDir().mkpath(moduleDir));

        {
            QFile qmldir(moduleDir + "/qmldir");
            QVERIFY(qmldir.open(QFile::WriteOnly | QFile::Text));
            QTextStream stream(&qmldir);
            stream << "module my.company.module\n"
                  << "MyType 1.0\n"
                  << "depends QtQuick auto\n";
            qmldir.close();
        }

        {
            QFile app(projectDir + "/app.qml");
            QVERIFY(app.open(QFile::WriteOnly | QFile::Text));
            QTextStream stream(&app);
            stream << "import my.company.module\n";
            app.close();
        }

        QMLQt6 customScaner(qmlRoot);
        QStringList results;
        QVERIFY2(customScaner.scan(results, projectDir),
                 "Failed to scan dotted qml module project dir");

        const auto expectedModule = QFileInfo(moduleDir).absoluteFilePath();
        QVERIFY2(results.contains(expectedModule),
                 ("Custom dotted qml module is not resolved to the project dir: " +
                  expectedModule).toUtf8().constData());
        QCOMPARE(customScaner.getPathFromImport("my/company/module"), expectedModule);
    }

}
