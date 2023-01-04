//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "disablerunscripttest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>
#include "modules.h"

void DisableRunScriptTest::test() {

    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif

    auto comapareTreeqif = TestModule.onlyC();
    comapareTreeqif -= utils.createTree({DISTRO_DIR + "/TestOnlyC.sh",
                                         DISTRO_DIR + "/TestOnlyC.bat"});
    // Run deploy installer
    runTestParams({"-bin", bin, "clear", "-libDir", TestQtDir + "/bin",
                   "-disableRunScript", "TestOnlyC"}, &comapareTreeqif);
}

