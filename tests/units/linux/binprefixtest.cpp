//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "binprefixtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void BinPrefixTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/bin/qt.conf"
                });
    QString target = TestBinDir + "TestOnlyC";
    QString targetWithoutPrefix = "TestOnlyC";

#else
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "/qt.conf"});
    QString target = TestBinDir + "TestOnlyC.exe";
    QString targetWithoutPrefix = "TestOnlyC.exe";

#endif

    runTestParams({"-bin", target, "force-clear"}, &comapareTree);

    runTestParams({"-bin", targetWithoutPrefix,
                   "-binPrefix", TestBinDir,
                   "force-clear"}, &comapareTree);
}
