//#
//# Copyright (C) 2020-2023 QuasarApp.
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

    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/bin/TestOnlyC",
                 "./" + DISTRO_DIR + "/bin/TestCPPOnly",
                 "./" + DISTRO_DIR + "/bin/QtWidgetsProject",
                 "./" + DISTRO_DIR + "/bin/TestQMLWidgets",
                 "./" + DISTRO_DIR + "/bin/QMLFileDialog",
                 "./" + DISTRO_DIR + "/bin/basic",
                 "./" + DISTRO_DIR + "/bin/quicknanobrowser",
                 "./" + DISTRO_DIR + "/bin/webui"});


    runTestParams({"-extraData", TestBinDir, "clear",
                   "noCheckRPATH", "noCheckPATH", "noQt"}, &comapareTree);


    comapareTree = TestModule.replace(comapareTree, {
                                          {"DistributionKit/bin",
                                           "DistributionKit/myExtraData/bin"}});

    runTestParams({"-extraData", TestBinDir, "clear",
                   "noCheckRPATH", "noCheckPATH", "noQt",
                   "-extraDataOut", "myExtraData"}, &comapareTree);
}
