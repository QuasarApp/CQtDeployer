//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "QIFWtestmulti.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void QIFWMultiTest::test() {
    TestUtils utils;
#ifdef Q_OS_UNIX
    QString target1 = TestBinDir + "TestCPPOnly";
    QString target2 = TestBinDir + "TestOnlyC";

    auto comapareTreeMulti = utils.createTree({
                                                  "./" + DISTRO_DIR + "/InstallerApplication.run",
                                                  "./" + DISTRO_DIR + "/InstallerApplication.run.md5",

                                              });

#else
    QString target1 = TestBinDir + "TestCPPOnly.exe";
    QString target2 = TestBinDir + "TestOnlyC.exe";

    auto comapareTreeMulti = utils.createTree({
                                                  "./" + DISTRO_DIR + "/InstallerApplication.exe",
                                                  "./" + DISTRO_DIR + "/InstallerApplication.exe.md5",

                                              });

#endif
    QString bin = target1;
    bin += "," + target2;

    auto packageString = "/package1/;" + QFileInfo(target1).absoluteFilePath() + ",/package2/;" + QFileInfo(target2).absoluteFilePath();
    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", packageString,
                   "qif", "qifFromSystem"}, &comapareTreeMulti, true);
}
