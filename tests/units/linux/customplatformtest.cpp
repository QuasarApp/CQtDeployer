//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "customplatformtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>
#include "modules.h"

void CustomPlatformTest::test() {
    TestUtils utils;

    auto compareTree = TestModule.onlyC();

#ifdef Q_OS_UNIX
    QString bin = {TestBinDir + "TestOnlyC"};
    QString platform = "linux_x86_64";

#else
    QString bin = {TestBinDir + "TestOnlyC.exe"};
    QString platform = "win_x86_64";

    compareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",
                    "./" + DISTRO_DIR + "/qt.conf"
                }
                );

#endif

    runTestParams({
                      "-bin", bin,
                      "clear",
                      "-platform", platform,
                  }, &compareTree
                  );



    runTestParams({
                      "-bin", bin,
                      "clear",
                      "-platform", "GeneralFile",
                  }, nullptr, false, false, exitCodes::PrepareError
                  );

}
