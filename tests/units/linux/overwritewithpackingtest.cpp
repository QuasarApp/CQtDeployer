//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "overwritewithpackingtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void OverwiriteWithPackingTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC," + TestBinDir + "TestCPPOnly";

#else
    QString bin = TestBinDir + "TestOnlyC.exe," + TestBinDir + "TestCPPOnly.exe";

#endif
#ifdef Q_OS_UNIX
    auto comapareTreeqif = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/InstallerTest.run",
                    "./" + DISTRO_DIR + "/InstallerTest.run.md5",
                });
#else
    auto comapareTreeqif = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/InstallerTest.exe",
                    "./" + DISTRO_DIR + "/InstallerTest.exe.md5",

                });

#endif

    runTestParams({"-bin", bin,
                   "force-clear",
                   "noOverwrite",
                   "qif",
                   "qifFromSystem",
                   "-name", "Test"}, &comapareTreeqif);

}
