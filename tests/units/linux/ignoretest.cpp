//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "ignoretest.h"
#include "modules.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void IgnoreTest::test() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "QtWidgetsProject";
    QString qmake = TestQtDir + "bin/qmake";
    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/QtWidgetsProject.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/QtWidgetsProject",
                });

#else
    QString bin = TestBinDir + "QtWidgetsProject.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/QtWidgetsProject.exe",
                    "./" + DISTRO_DIR + "/QtWidgetsProject.bat",
                #if defined(Q_OS_WIN64)
                    "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
                #else
                    "./" + DISTRO_DIR + "/libgcc_s_dw2-1.dll",
                #endif
                    "./" + DISTRO_DIR + "/libstdc++-6.dll",
                    "./" + DISTRO_DIR + "/libwinpthread-1.dll"

                });

#endif


    if (!TestQtDir.contains("Qt6")) {

#ifdef Q_OS_UNIX
        bin = TestBinDir + "QtWidgetsProject";
        qmake = TestQtDir + "bin/qmake";

#else
        bin = TestBinDir + "QtWidgetsProject.exe";
        qmake = TestQtDir + "bin/qmake.exe";

#endif
    }


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-ignore", "Qt6"}, &comapareTree);



#ifdef Q_OS_UNIX
    auto removeTree = utils.createTree({
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_hangul.so",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_openwnn.so",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_pinyin.so",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_tcime.so",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_thai.so",
                                           "./" + DISTRO_DIR + "/plugins/platforminputcontexts/libqtvirtualkeyboardplugin.so",
                                           "./" + DISTRO_DIR + "/lib/libQt6VirtualKeyboard.so",

                                       });
#else
    auto removeTree = utils.createTree({
                                           "./" + DISTRO_DIR + "/Qt6VirtualKeyboard.dll",
                                           "./" + DISTRO_DIR + "/plugins/platforminputcontexts/qtvirtualkeyboardplugin.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_hangul.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_openwnn.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_pinyin.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_tcime.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_thai.dll"
                                       });

#endif

    comapareTree = TestModule.qtLibs() - removeTree;

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-ignore", "VirtualKeyboard"}, &comapareTree);


}
