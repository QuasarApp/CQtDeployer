//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "ignoreenvwithlibdirtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void IgnoreEnvWithLibDirTest::test() {
#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif

    QVERIFY(QDir().mkdir("libDirtest"));

    // Run deploy installer
    runTestParams({"-bin", bin, "clear",
                   "-targetDir", "./libDirtest",
                   "-libDir", "./libDirtest"}, nullptr, false, false,
                  exitCodes::PrepareError);

    QVERIFY(QDir().rmdir("libDirtest"));
}
