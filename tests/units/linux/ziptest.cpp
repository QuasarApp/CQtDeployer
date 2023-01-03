//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "ziptest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void ZIPTest::test() {
    
    TestUtils utils;

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/TestOnlyC.zip",
                                             "./" + DISTRO_DIR + "/TestOnlyC.zip.md5",
                                         });



#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif

    runTestParams({"-bin", bin, "clear" ,
                   "zip", "verbose"}, &comapareTree, true);

    // test clear for zip
    runTestParams({"clear", "verbose"}, nullptr, true);

}

