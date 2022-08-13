//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "targetdirtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void TargetDirTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "Z/bin/TestOnlyC",
                 "./" + DISTRO_DIR + "Z/bin/qt.conf",
                 "./" + DISTRO_DIR + "Z/TestOnlyC.sh"});
#else
    QString bin = TestBinDir + "TestOnlyC.exe";

    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "Z/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "Z/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "Z/qt.conf"});
#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-targetDir", "./" + DISTRO_DIR + "Z"
                  }, &comapareTree);
}

void deploytest::testSystemLib() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    QString qmake = TestQtDir + "bin/qmake";

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/lib/systemLibs/libgcc_s.so",
                    "./" + DISTRO_DIR + "/lib/systemLibs/libstdc++.so"
                });

#else
    QString bin = TestBinDir + "TestOnlyC.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",
                #if defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "/systemLibs/libgcc_s_seh-1.dll",
                #else
                    "./" + DISTRO_DIR + "/systemLibs/libgcc_s_dw2-1.dll",
                #endif
                    "./" + DISTRO_DIR + "/systemLibs/libstdc++-6.dll",
                    "./" + DISTRO_DIR + "/systemLibs/libwinpthread-1.dll",
                    "./" + DISTRO_DIR + "/systemLibs/msvcrt.dll",
                    "./" + DISTRO_DIR + "/qt.conf"
                });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "deploySystem",
                   "-qmake", qmake,
                  }, &comapareTree);

#ifdef Q_OS_WIN
    bin = TestBinDir + "QtWidgetsProject.exe";

    comapareTree += TestModule.qtLibs();

    comapareTree -= utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",
                #if defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "/systemLibs/libgcc_s_seh-1.dll",
                #else
                    "./" + DISTRO_DIR + "/systemLibs/libgcc_s_dw2-1.dll",
                #endif
                    "./" + DISTRO_DIR + "/systemLibs/libstdc++-6.dll",
                    "./" + DISTRO_DIR + "/systemLibs/libwinpthread-1.dll",

                });

    comapareTree += utils.createTree(
                {
                    "./" + DISTRO_DIR + "/systemLibs/msvcrt.dll",
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/systemLibs/mpr.dll",
                    "./" + DISTRO_DIR + "/systemLibs/profapi.dll",
                    "./" + DISTRO_DIR + "/systemLibs/rpcrt4.dll",
                    "./" + DISTRO_DIR + "/systemLibs/shell32.dll",
                    "./" + DISTRO_DIR + "/systemLibs/userenv.dll",
                    "./" + DISTRO_DIR + "/systemLibs/uxtheme.dll",
                    "./" + DISTRO_DIR + "/systemLibs/version.dll",
                    "./" + DISTRO_DIR + "/systemLibs/ucrtbase.dll",
                    "./" + DISTRO_DIR + "/systemLibs/oleaut32.dll",
                    "./" + DISTRO_DIR + "/systemLibs/bcryptprimitives.dll",
                    "./" + DISTRO_DIR + "/systemLibs/msvcp_win.dll",
                    "./" + DISTRO_DIR + "/systemLibs/wtsapi32.dll",
                    "./" + DISTRO_DIR + "/systemLibs/combase.dll",
                #if !defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "/systemLibs/sspicli.dll",
                    "./" + DISTRO_DIR + "/systemLibs/cryptbase.dll",
                #endif

                });


#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    comapareTree += utils.createTree(
                {
                    "./" + DISTRO_DIR + "/systemLibs/d3d11.dll",
                    "./" + DISTRO_DIR + "/systemLibs/dxgi.dll",
                    "./" + DISTRO_DIR + "/systemLibs/win32u.dll",
                });
#endif


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "deploySystem"
                  }, &comapareTree);


#endif
}
