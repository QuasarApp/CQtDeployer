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
    comapareTree = utils.createTree(
        {
            "./" + DISTRO_DIR + "/lib/libicutu.so.56.1",
        });

    runTestParams({"-bin", bin, "clear" ,
                   "-extraDepends", "libicutu.so.56.1"}, &comapareTree);


    runTestParams({"-bin", bin, "clear" ,
                   "-extraDepends", "TestOnlyC;libicutu.so.56.1"}, &comapareTree);

}
