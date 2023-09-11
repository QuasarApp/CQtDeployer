//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "targetdirtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void TargetDirTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "Z/bin/TestOnlyC",
                 "./" + DISTRO_DIR + "Z/bin/qt.conf",
                 "./" + DISTRO_DIR + "Z/TestOnlyC.sh"});
#else
    QString bin = TestBinDir + "TestOnlyC.exe";

    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "Z/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "Z/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "Z/qt.conf"});
#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-targetDir", "./" + DISTRO_DIR + "Z"
                  }, &comapareTree);
}
