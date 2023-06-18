//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "extradependstest.h"
#include "modules.h"

ExtraDependsTest::ExtraDependsTest() {

}

void ExtraDependsTest::test() {
    TestUtils utils;

    QString bin = TestBinDir + "TestOnlyC";

    auto comapareTree = TestModule.onlyC();

    // should be without icu libs
    runTestParams({"-bin", bin, "clear",
                   "-libDir", QT_BASE_DIR,
                   "-recursiveDepth", "4"}, &comapareTree);

    comapareTree += utils.createTree(
        {
            "./" + DISTRO_DIR + "/lib/libicutu.so.56.1",
            "./" + DISTRO_DIR + "/lib/libicuuc.so",
            "./" + DISTRO_DIR + "/lib/libicui18n.so",
            "./" + DISTRO_DIR + "/lib/libicudata.so",

        });

    // this cases should contains all icu libs.
    runTestParams({"-bin", bin, "clear",
                   "-libDir", QT_BASE_DIR,
                   "-recursiveDepth", "4",
                   "-extraDepends", "libicutu.so.56.1"}, &comapareTree);


    runTestParams({"-bin", bin, "clear" ,
                   "-libDir", QT_BASE_DIR,
                   "-recursiveDepth", "4",
                   "-extraDepends", "TestOnlyC;libicutu.so.56.1"}, &comapareTree);

}
