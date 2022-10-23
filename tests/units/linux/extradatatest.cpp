//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "extradatatest.h"
#include "modules.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void ExtraDataTest::test() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/bin/TestOnlyC",
                 "./" + DISTRO_DIR + "/bin/TestCPPOnly",
                 "./" + DISTRO_DIR + "/bin/QtWidgetsProject",
                 "./" + DISTRO_DIR + "/bin/TestQMLWidgets",
                 "./" + DISTRO_DIR + "/bin/basic",
                 "./" + DISTRO_DIR + "/bin/quicknanobrowser",
                 "./" + DISTRO_DIR + "/bin/webui"});
#else
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/b    in/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/bin/TestCPPOnly.exe",
                 "./" + DISTRO_DIR + "/bin/QtWidgetsProject.exe",
                 "./" + DISTRO_DIR + "/bin/TestQMLWidgets.exe",
                 "./" + DISTRO_DIR + "/bin/basic.exe"});
#endif


    runTestParams({"-extraData", TestBinDir, "clear",
                   "noCheckRPATH", "noCheckPATH", "noQt"}, &comapareTree);


    comapareTree = TestModule.replace(comapareTree, {
                                          {"DistributionKit/bin",
                                           "DistributionKit/myExtraData/bin"}});

    runTestParams({"-extraData", TestBinDir, "clear",
                   "noCheckRPATH", "noCheckPATH", "noQt",
                   "-extraDataOut", "myExtraData"}, &comapareTree);
}
