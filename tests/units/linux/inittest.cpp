//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "inittest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void InitTest::test() {
    TestUtils utils;

    runTestParams({"init"});

    QVERIFY(QFile(DEFAULT_COFIGURATION_FILE).remove());

    runTestParams({"-init", "multi"});


    QVERIFY(QFile(DEFAULT_COFIGURATION_FILE).remove());

    runTestParams({"-init", "single"});

    QVERIFY(QFile(DEFAULT_COFIGURATION_FILE).remove());

}
