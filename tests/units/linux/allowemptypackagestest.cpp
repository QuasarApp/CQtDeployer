//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "allowemptypackagestest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void AllowEmptyPackagesTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif
    runTestParams({"-bin", bin, "force-clear",
                   "-prefix", "package;prefix"}, nullptr, false, false,
                  exitCodes::PrepareError);

    runTestParams({"-bin", bin, "force-clear",
                   "-prefix", "package;prefix",
                   "allowEmptyPackages"});
}
