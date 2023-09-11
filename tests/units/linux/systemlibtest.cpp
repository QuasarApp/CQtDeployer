//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "systemlibtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void SystemLibTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestCPPOnly";
    QString qmake = TestQtDir + "bin/qmake";

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestCPPOnly.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/TestCPPOnly",
                    "./" + DISTRO_DIR + "/lib/systemLibs/libgcc_s.so",
                    "./" + DISTRO_DIR + "/lib/systemLibs/libstdc++.so"
                });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "deploySystem",
                   "-qmake", qmake,
                  }, &comapareTree, true);


}
