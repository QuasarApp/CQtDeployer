//#
//# Copyright (C) 2020-2024 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "hidenfilestest.h"

#include <filemanager.h>



void HidenFilesTest::test() {
#ifdef Q_OS_UNIX
    TestUtils utils;


    QDir("tstAssets").removeRecursively();
    FileManager files;
    files.cp(":/testRes/.hidenAssets", "tstAssets");


    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/Application.deb",
                                             "./" + DISTRO_DIR + "/Application.deb.md5",

                                         });

    runTestParams({"-extraData", "tstAssets", "clear" ,
                   "deb", "verbose"}, &comapareTree, true);

    // test clear for deb
    runTestParams({"clear", "verbose"}, nullptr, true);

    QDir("tstAssets").removeRecursively();

#endif
}
