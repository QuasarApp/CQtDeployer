//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "QIFWtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void QIFWTest::test() {
    TestUtils utils;
#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";

    QString qmake = TestQtDir + "bin/qmake";
    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.run",
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.run.md5",
                                         });

#else
    QString bin = TestBinDir + "TestOnlyC.exe";

    QString qmake = TestQtDir + "bin/qmake.exe";
    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.exe",
                                             "./" + DISTRO_DIR + "/InstallerTestOnlyC.exe.md5",

                                         });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "qif", "qifFromSystem",
                   "-qifStyle", "quasar",
                   "-qifBanner", TestBinDir + "/../../res/CQtDeployer_banner_web.png",
                   "-qifLogo", TestBinDir + "/../../res/CQtDeployer defaultIcon_web.png"}, &comapareTree, true);
}
