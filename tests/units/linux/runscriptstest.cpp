//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "runscriptstest.h"
#include "modules.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void RunScriptsTest::test() {
    
    TestUtils utils;
    auto comapareTree = TestModule.onlyC();

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";

    QFile f(":/testResurces/testRes/customRunScript.sh");
    QVERIFY(f.open(QIODevice::ReadOnly));
    auto etalonData = f.readAll();
    f.close();

    runTestParams({"-bin", bin,
                   "force-clear",
                   "-libOut", "lib",
                   "-runScript", "TestOnlyC;:/testResurces/testRes/customRunScript.sh"}, &comapareTree);

    f.setFileName(DISTRO_DIR + "/TestOnlyC.sh");
    QVERIFY(f.open(QIODevice::ReadOnly));
    auto deployData = f.readAll();

    QVERIFY(deployData == etalonData);
#else
    QString bin = TestBinDir + "TestOnlyC.exe";

    QFile f(":/testResurces/testRes/customRunScript.sh");
    QVERIFY(f.open(QIODevice::ReadOnly));
    auto etalonData = f.readAll();
    f.close();

    runTestParams({"-bin", bin,
                   "force-clear",
                   "-libOut", "lib",
                   "-runScript", "TestOnlyC.exe;:/testResurces/testRes/customRunScript.sh"}, nullptr);

    f.setFileName(DISTRO_DIR + "/TestOnlyC.bat");
    QVERIFY(f.open(QIODevice::ReadOnly));
    auto deployData = f.readAll();

    QVERIFY(deployData == etalonData);
#endif

}
