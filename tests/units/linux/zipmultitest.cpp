//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "zipmultitest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void ZIPMultiTest::test() {
    
    TestUtils utils;

    auto comapareTreeMulti = utils.createTree({
                                                  "./" + DISTRO_DIR + "/package1.zip",
                                                  "./" + DISTRO_DIR + "/package2.zip",
                                                  "./" + DISTRO_DIR + "/package1.zip.md5",
                                                  "./" + DISTRO_DIR + "/package2.zip.md5",

                                              });

#ifdef Q_OS_UNIX
    QString target1 = TestBinDir + "TestCPPOnly";
    QString target2 = TestBinDir + "TestOnlyC";

#else
    QString target1 = TestBinDir + "TestCPPOnly.exe";
    QString target2 = TestBinDir + "TestOnlyC.exe";
#endif
    QString bin = target1;
    bin += "," + target2;

    auto packageString = "/package1/;" + QFileInfo(target1).absoluteFilePath() + ",/package2/;" + QFileInfo(target2).absoluteFilePath();
    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", packageString,
                   "zip"}, &comapareTreeMulti, true);
}
