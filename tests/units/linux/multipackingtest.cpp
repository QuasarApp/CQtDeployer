//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "multipackingtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void MultiPackingTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/TestOnlyC.zip",
                                             "./" + DISTRO_DIR + "/TestOnlyC.deb",
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.run",
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.run.md5",
                                             "./" + DISTRO_DIR + "/TestOnlyC.zip.md5",
                                             "./" + DISTRO_DIR + "/TestOnlyC.deb.md5",

                                         });

    QString bin = TestBinDir + "TestOnlyC";

    runTestParams({"-bin", bin, "clear" ,
                   "zip",
                   "qif", "qifFromSystem",
                   "deb",
                   "verbose"}, &comapareTree, true);

#else
    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/TestOnlyC.zip",
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.exe",
                                             "./" + DISTRO_DIR + "/TestOnlyC.zip.md5",
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.exe.md5",
                                         });
    QString bin = TestBinDir + "TestOnlyC.exe";

    runTestParams({"-bin", bin, "clear" ,
                   "zip",
                   "qif", "qifFromSystem",
                   "verbose"}, &comapareTree, true);

#endif
}
