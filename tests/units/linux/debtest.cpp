//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "debtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void DEBTest::test() {
#ifdef Q_OS_UNIX
    TestUtils utils;

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/TestOnlyC.deb",
                                             "./" + DISTRO_DIR + "/TestOnlyC.deb.md5",

                                         });

    QString bin = TestBinDir + "TestOnlyC";

    runTestParams({"-bin", bin, "clear" ,
                   "deb", "verbose"}, &comapareTree, true);

    // test clear for deb
    runTestParams({"clear", "verbose"}, nullptr, true);

#endif
}
