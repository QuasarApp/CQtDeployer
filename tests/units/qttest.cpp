//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "qttest.h"
#include "modules.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void QtTest::test() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "QtWidgetsProject";
    QString qmake = TestQtDir + "bin/qmake";

#else
    QString bin = TestBinDir + "QtWidgetsProject.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

#endif


    auto comapareTree = TestModule.qtLibs();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake}, &comapareTree);

    // test auto detection of detection qmake from PATH
    runTestParams({"-bin", bin, "clear", "noCheckRPATH"}, &comapareTree);

#ifdef Q_OS_UNIX
    // test auto detection of detection qmake from RPATH
    runTestParams({"-bin", bin, "clear", "noCheckPATH"}, &comapareTree);
#endif


    comapareTree = TestModule.qtWithoutTr();


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake, "noTranslations"}, &comapareTree);


    comapareTree = TestModule.qmlLibs();

#ifdef Q_OS_UNIX
    bin = TestBinDir + "TestQMLWidgets";
#else
    bin = TestBinDir + "TestQMLWidgets.exe";

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets"}, &comapareTree);

}
