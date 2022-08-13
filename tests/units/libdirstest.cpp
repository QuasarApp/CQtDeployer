//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "libdirstest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void LibDirTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    QString extraPath = "/usr/lib,/lib";

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/TestOnlyC"
                });

#else
    QString bin = TestBinDir + "TestOnlyC.exe";
    QString extraPath = TestQtDir;

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",

                });

#endif




    runTestParams({"-bin", bin, "clear" ,
                   "-libDir", extraPath,
                  }, &comapareTree);


#ifdef Q_OS_UNIX
    comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/lib/libstdc++.so",
                    "./" + DISTRO_DIR + "/lib/libgcc_s.so",
                    "./" + DISTRO_DIR + "/lib/ld-linux-x86-64.so",
                    "./" + DISTRO_DIR + "/lib/libc.so",
                    "./" + DISTRO_DIR + "/lib/libm.so",
                });

    auto comapareTreeExtraLib = utils.createTree(
                {
                    "./" + DISTRO_DIR + "2/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "2/bin/qt.conf",
                    "./" + DISTRO_DIR + "2/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "2/lib/libstdc++.so",
                    "./" + DISTRO_DIR + "2/lib/libgcc_s.so"
                });

#else
    comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",
                #if defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
                #else
                    "./" + DISTRO_DIR + "/libgcc_s_dw2-1.dll",
                #endif
                    "./" + DISTRO_DIR + "/libwinpthread-1.dll",
                    "./" + DISTRO_DIR + "/libstdc++-6.dll",

                });

    auto comapareTreeExtraLib = utils.createTree(
                {
                    "./" + DISTRO_DIR + "2/qt.conf",
                    "./" + DISTRO_DIR + "2/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "2/TestOnlyC.bat",
                #if defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "2/libgcc_s_seh-1.dll",
                #else
                    "./" + DISTRO_DIR + "2/libgcc_s_dw2-1.dll",
                #endif
                    "./" + DISTRO_DIR + "2/libstdc++-6.dll",

                });

#endif


    runTestParams({"-bin", bin, "clear" ,
                   "-libDir", extraPath,
                   "-recursiveDepth", "5",
                   "noCheckRPATH, noCheckPATH", "noQt"}, &comapareTree, true);

    runTestParams({"-bin", bin, "clear" ,
                   "-targetDir", "./" + DISTRO_DIR + "2",
                   "-extraLibs", "stdc,gcc",
                   "noCheckRPATH, noCheckPATH", "noQt"}, &comapareTreeExtraLib, true);

    //task #258
    //https://github.com/QuasarApp/CQtDeployer/issues/258


#ifdef Q_OS_UNIX
    comapareTreeExtraLib = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/lib/libstdc++.so",
                    "./" + DISTRO_DIR + "/lib/libgcc_s.so"
                });
    extraPath = "./" + DISTRO_DIR + "2/lib";


#else

    comapareTreeExtraLib = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",
                #if defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
                #else
                    "./" + DISTRO_DIR + "/libgcc_s_dw2-1.dll",
                #endif
                    "./" + DISTRO_DIR + "/libstdc++-6.dll",

                });
    extraPath = "./" + DISTRO_DIR + "2";

#endif
    runTestParams({"-bin", bin, "clear" ,
                   "-libDir", extraPath,
                   "noCheckRPATH, noCheckPATH", "noQt"}, &comapareTreeExtraLib, true);

    QDir(extraPath).removeRecursively();

}
