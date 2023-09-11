//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "QIFWtestcustom.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void QIFWCustomTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestCPPOnly" + "," + TestBinDir + "TestOnlyC";

    QString qmake = TestQtDir + "bin/qmake";

    auto comapareTreeCustom = utils.createTree({
                                                   "./" + DISTRO_DIR + "/Installerorg.qtproject.ifw.example.stylesheet.run",
                                                   "./" + DISTRO_DIR + "/Installerorg.qtproject.ifw.example.stylesheet.run.md5",

                                               });
#else
    QString bin = TestBinDir + "TestCPPOnly.exe" + "," + TestBinDir + "TestOnlyC.exe";

    QString qmake = TestQtDir + "bin/qmake.exe";
    auto comapareTreeCustom = utils.createTree({
                                                   "./" + DISTRO_DIR + "/Installerorg.qtproject.ifw.example.stylesheet.exe",
                                                   "./" + DISTRO_DIR + "/Installerorg.qtproject.ifw.example.stylesheet.exe.md5",
                                               });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qif", TestBinDir + "/../../UnitTests/testRes/QIFCustomTemplate",
                   "-name", "org.qtproject.ifw.example.stylesheet",
                   "qifFromSystem"}, &comapareTreeCustom, true);
}
