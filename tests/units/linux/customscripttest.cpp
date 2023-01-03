//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "customscripttest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void CustomScriptTest::test() {
    
    TestUtils utils;

#ifdef Q_OS_UNIX
    QFile f("./" + DISTRO_DIR + "/bin/TestOnlyC");
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/bin/TestOnlyC",
                 "./" + DISTRO_DIR + "/bin/qt.conf",
                 "./" + DISTRO_DIR + "/TestOnlyC.sh"});
    QString bin = TestBinDir + "TestOnlyC";
    QString scriptPath = "./" + DISTRO_DIR + "/TestOnlyC.sh";

#else
    QFile f("./" + DISTRO_DIR + "/TestOnlyC.exe");
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "/qt.conf"});
    QString bin = TestBinDir + "TestOnlyC.exe";
    QString scriptPath = "./" + DISTRO_DIR + "/TestOnlyC.bat";


#endif

    runTestParams({"-bin", bin, "force-clear", "noOverwrite", "-libOut", "lib"}, &comapareTree);

    QFile script(scriptPath);
    QVERIFY(script.open(QIODevice::ReadOnly));
    auto scriptText = script.readAll();

    QVERIFY(!scriptText.contains("Begin Custom Script"));

    script.close();

    runTestParams({"-bin", bin, "force-clear", "noOverwrite",
                   "-libOut", "lib",
                   "-customScript", "echo 'this is test script'"}, &comapareTree);

    QVERIFY(script.open(QIODevice::ReadOnly));

    scriptText = script.readAll();

    QVERIFY(scriptText.contains("Begin Custom Script"));
    QVERIFY(scriptText.contains("echo 'this is test script'"));
    QVERIFY(scriptText.contains("End Custom Script"));

    script.close();



}

