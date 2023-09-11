//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "virtualkeybordtest.h"
#include "modules.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void VirtualKeyBoardTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "basic";
    QString qmake = TestQtDir + "bin/qmake";
#else
    QString bin = TestBinDir + "basic.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";
#endif
    auto comapareTree = TestModule.qmlVirtualKeyBoadrLibs();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../virtualkeyboard"}, &comapareTree);
}
