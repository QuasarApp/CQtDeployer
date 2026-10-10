//#
//# Copyright (C) 2020-2026 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "qmlnativeplugintest.h"

#include <QDir>
#include <QFileInfo>
#include <QFile>

void QmlNativePluginTest::test() {

    qInfo() << "Testing deployment of auto-generated Qt6 QML plugin module (QMLNativePlugin)";

    // Deploy the QMLNativePlugin test case. The QML project build dir is passed as the -qmlDir
    // option so the deployer can scan it for auto-generated QML modules (created with
    // qt_add_qml_module()).
    runTestParams(
        {"-bin", TestBinDir + "QMLNativePlugin",
         "clear",
         "-qmlDir", TestBuildDir + "QMLNativePlugin"});

    // A module created with qt_add_qml_module() is identified by the "module <uri>" line of its
    // qmldir file, not by the directory name. The generated module must be deployed to
    // <targetDir>/qml/<moduleUri>/ using that module uri.
    auto qmlModuleDir = "./" + DISTRO_DIR + "/qml/NativePluginModule";
    qInfo() << "Checking auto-generated QML module dir:" << qmlModuleDir;
    QVERIFY2(QDir(qmlModuleDir).exists(),
             qPrintable("The auto-generated QML module dir is missing: " + qmlModuleDir));

    // The qmldir that identifies the module.
    QVERIFY2(QFile::exists(qmlModuleDir + "/qmldir"),
             qPrintable("Missing module qmldir: " + qmlModuleDir + "/qmldir"));

    // The type-registered plugin library (generated because the module declares QML_ELEMENT).
    QVERIFY2(QFile::exists(qmlModuleDir + "/libNativePluginModule.so"),
             qPrintable("Missing generated module library: " + qmlModuleDir + "/libNativePluginModule.so"));

    // The plugin library that registers the native C++ types (generated because the module has
    // native sources and a qmldir Plugin line).
    QVERIFY2(QFile::exists(qmlModuleDir + "/libNativePluginModuleplugin.so"),
             qPrintable("Missing native type plugin: " + qmlModuleDir + "/libNativePluginModuleplugin.so"));

    // The types file used for code completion.
    QVERIFY2(QFile::exists(qmlModuleDir + "/NativePluginModule.qmltypes"),
             qPrintable("Missing qmltypes: " + qmlModuleDir + "/NativePluginModule.qmltypes"));

    // The generated module build dir also contains CMake/build artifacts (CMakeFiles, *_autogen,
    // Makefile, qmldir.cpp, ...). None of those may be copied into the deployment.
    QVERIFY2(!QDir(qmlModuleDir).exists("CMakeFiles"),
             "Build artifact CMakeFiles must not be deployed");
    QVERIFY2(!QFile::exists(qmlModuleDir + "/Makefile"),
             "Build artifact Makefile must not be deployed");
    QVERIFY2(!QFile::exists(qmlModuleDir + "/qmldir.cpp"),
             "Generated source qmldir.cpp must not be deployed");
    QVERIFY2(!QDir(qmlModuleDir).exists("QMLNativePlugin_autogen"),
             "Autogen build dir must not be deployed");
}