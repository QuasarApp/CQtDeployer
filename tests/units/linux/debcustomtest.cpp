//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "debcustomtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void DEBCustomTest::test() {
#ifdef Q_OS_UNIX

    TestUtils utils;

    QString bin = TestBinDir + "TestOnlyC";

    auto comapareTreeCustom = utils.createTree({
                                                   "./" + DISTRO_DIR + "/chrome.deb",
                                                   "./" + DISTRO_DIR + "/chrome.deb.md5",

                                               });

    runTestParams({"-bin", bin, "clear" ,
                   "-deb", TestBinDir + "/../../UnitTests/testRes/DEBCustomTemplate",
                   "-name", "chrome"},
                  &comapareTreeCustom, true);
#endif
}
