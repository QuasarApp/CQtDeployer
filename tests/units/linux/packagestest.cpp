//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "packagestest.h"
#include "modules.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void PacakgesTest::test() {
 TestUtils utils;

#ifdef Q_OS_UNIX
    QFile f("./" + DISTRO_DIR + "/bin/TestOnlyC");
    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/package/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/package/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/package/bin/qt.conf"
                });
    QString target1 = TestBinDir + "TestOnlyC";

#else
    QFile f("./" + DISTRO_DIR + "/TestOnlyC.exe");
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/package/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/package/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "/package/qt.conf"});
    QString target1 = TestBinDir + "TestOnlyC.exe";

#endif
    QString bin = target1;

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "/package/;TestOn"}, &comapareTree);

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "/package/;" + QFileInfo(target1).absoluteFilePath()}, &comapareTree);

#ifdef Q_OS_UNIX
    QString target2 = TestBinDir + "TestCPPOnly";
    QString target3 = TestBinDir + "QtWidgetsProject";

#else
    QString target2 = TestBinDir + "TestCPPOnly.exe";
    QString target3 = TestBinDir + "QtWidgetsProject.exe";

#endif
    bin += "," + target2;
    bin += "," + target3;

    comapareTree = TestModule.testDistroLibs(DISTRO_DIR);

    comapareTree = TestModule.replace(comapareTree, {
                                         {"package1/bin/", "package1/superBin/"},

                                    });

    auto packageString = "package1;" + QFileInfo(target1).absoluteFilePath() +
                         ",package2/pack;" + QFileInfo(target2).absoluteFilePath();
    runTestParams({"-bin", bin, "force-clear",
                   "-binOut", "package1;superBin",
                   "-targetPackage", packageString}, &comapareTree);

}
