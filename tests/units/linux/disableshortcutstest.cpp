//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "disableshortcutstest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void DisableShortcutsTest::test() {

    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif

    auto cb = [](const DeployConfig * config){
        const auto list = config->getTargetsListByFilter("TestOnlyC");
        // Check shrtcut option. all targets should be return false.
        for (auto target: list) {
            QVERIFY(!target->getShortCut());
        }
    };

    // Run deploy installer
    runTestParams({"-bin", bin, "clear",
                   "-disableShortCut", "TestOnlyC",
                   "qif", "qifFromSystem"},
                  nullptr,
                  false,
                  false,
                  exitCodes::Good,
                  cb);


}
