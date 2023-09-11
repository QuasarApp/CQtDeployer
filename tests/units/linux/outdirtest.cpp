//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "outdirtest.h"
#include "modules.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>

void OutDirTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestQMLWidgets";
    QString qmake = TestQtDir + "bin/qmake";

#else
    QString bin = TestBinDir + "TestQMLWidgets.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";


#endif

    auto comapareTree = TestModule.qmlLibs(DISTRO_DIR);

    comapareTree = TestModule.replace(comapareTree, {
                                      {"/bin/", "/lol/"},
                                      {"/lib/", "/lolLib/"},
                                      {"/translations/", "/lolTr/"},
                                      {"/plugins/", "/p/"},
                                      {"/qml/", "/q/"},
                                     });


    runTestParams({"-bin", bin, "clear" ,
                   "-binOut", "/lol",
                   "-libOut", "/lolLib",
                   "-trOut", "/lolTr",
                   "-pluginOut", "/p",
                   "-qmlOut", "/q",
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets"
                  }, &comapareTree);

    QFile file;

    file.setFileName("./" + DISTRO_DIR + "/lol/qt.conf");

    QVERIFY(file.open(QIODevice::ReadOnly));

    auto runScript = file.readAll();
    file.close();

    QVERIFY(runScript.contains("Prefix= ./../"));
    QVERIFY(runScript.contains("Libraries= ./lolLib/"));
    QVERIFY(runScript.contains("Plugins= ./p/"));
    QVERIFY(runScript.contains("Imports= ./q/"));
    QVERIFY(runScript.contains("Translations= ./lolTr/"));
    QVERIFY(runScript.contains("Qml2Imports= ./q/"));

#ifdef Q_OS_WIN


    runTestParams({"-bin", bin, "clear" ,
                   "-binOut", "/lol",
                   "-libOut", "/lolLib",
                   "-trOut", "/lolTr",
                   "-pluginOut", "/p",
                   "-qmlOut", "/q",
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets"
                  }, &comapareTree);



    file.setFileName( "./" + DISTRO_DIR + "/TestQMLWidgets.bat");

    QVERIFY(file.open(QIODevice::ReadOnly));

    runScript = file.readAll();
    file.close();

    qDebug() << "runScript =" << runScript;

    QVERIFY(runScript.contains("SET BASE_DIR=%~dp0"));
    QVERIFY(runScript.contains("SET PATH=%BASE_DIR%\\lol\\;%BASE_DIR%\\lolLib\\;%PATH%"));
    QVERIFY(runScript.contains("start \"TestQMLWidgets\" /B \"%BASE_DIR%\\lol\\TestQMLWidgets.exe\" %*"));

    runTestParams({"-bin", TestBinDir + "TestOnlyC.exe", "clear",
                  }, nullptr);

    file.setFileName( "./" + DISTRO_DIR + "/TestOnlyC.bat");

    QVERIFY(file.open(QIODevice::ReadOnly));

    runScript = file.readAll();
    file.close();

    qDebug() << "runScript =" << runScript;

    QVERIFY(runScript.contains("call \"%BASE_DIR%\\TestOnlyC.exe\" %*"));

#endif

}
