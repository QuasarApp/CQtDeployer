//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "deploygeneralfilestest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void DeployGeneralFilesTest::test() {
     TestUtils utils;

    QString bin = TestBinDir + "/../../CMakeLists.txt";

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/bin/CMakeLists.txt",
                    "./" + DISTRO_DIR + "/bin/qt.conf",

                });

    runTestParams(
                {"-bin", bin,
                 "-binOut", "bin",
                 "force-clear"
                }, &comapareTree);

}
