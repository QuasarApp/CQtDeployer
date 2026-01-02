//#
//# Copyright (C) 2020-2026 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "webenginetest.h"

#ifdef USE_WEBENGINE
#include "modules.h"
#endif

#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void WebEngineTest::test() {
#ifdef USE_WEBENGINE
    TestUtils utils;

    QString bin = TestBinDir + "quicknanobrowser";
    QString qmake = TestQtDir + "bin/qmake";

    auto comapareTree = TestModule.qtWebEngine();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../quicknanobrowser"}, &comapareTree);


    bin = TestBinDir + "webui";
    qmake = TestQtDir + "bin/qmake";

    comapareTree = TestModule.qtWebEngineWidgets();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake}, &comapareTree);
#endif
}
