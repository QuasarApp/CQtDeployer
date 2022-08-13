//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "extradatatest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void ExtraDataTest::test() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/build/TestOnlyC",
                 "./" + DISTRO_DIR + "/build/TestCPPOnly",
                 "./" + DISTRO_DIR + "/build/QtWidgetsProject",
                 "./" + DISTRO_DIR + "/build/TestQMLWidgets",
                 "./" + DISTRO_DIR + "/build/basic",
                 "./" + DISTRO_DIR + "/build/quicknanobrowser",
                 "./" + DISTRO_DIR + "/build/webui"});
#else
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/build/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/build/TestCPPOnly.exe",
                 "./" + DISTRO_DIR + "/build/QtWidgetsProject.exe",
                 "./" + DISTRO_DIR + "/build/TestQMLWidgets.exe",
                 "./" + DISTRO_DIR + "/build/basic.exe"});
#endif


    runTestParams({"-extraData", TestBinDir, "clear",
                   "noCheckRPATH", "noCheckPATH", "noQt"}, &comapareTree);


    comapareTree = TestModule.replace(comapareTree, {
                                          {"DistributionKit/build",
                                           "DistributionKit/myExtraData/build"}});

    runTestParams({"-extraData", TestBinDir, "clear",
                   "noCheckRPATH", "noCheckPATH", "noQt",
                   "-extraDataOut", "myExtraData"}, &comapareTree);
}
