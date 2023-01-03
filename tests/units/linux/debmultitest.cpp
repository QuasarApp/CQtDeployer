//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "debmultitest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void DEBMultiTest::test() {
#ifdef Q_OS_UNIX
    TestUtils utils;

    auto comapareTreeMulti = utils.createTree({
                                                  "./" + DISTRO_DIR + "/package1.deb",
                                                  "./" + DISTRO_DIR + "/package2.deb",
                                                  "./" + DISTRO_DIR + "/package1.deb.md5",
                                                  "./" + DISTRO_DIR + "/package2.deb.md5",

                                              });

    QString target1 = TestBinDir + "TestCPPOnly";

    QString target2 = TestBinDir + "TestOnlyC";

    QString bin = target1;
    bin += "," + target2;

    auto packageString = "/package1/;" + QFileInfo(target1).absoluteFilePath() + ",/package2/;" + QFileInfo(target2).absoluteFilePath();
    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", packageString,
                   "deb"}, &comapareTreeMulti, true);
#endif
}
