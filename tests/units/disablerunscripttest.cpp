//#
//# Copyright (C) 2020-2022 QuasarApp.
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

void deploytest::testQifOut() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif

#ifdef Q_OS_UNIX
    auto result = utils.createTree({{DISTRO_DIR + "/QIF_OUT.exe"}, {DISTRO_DIR + "/QIF_OUT.exe.md5"},
                                    {DISTRO_DIR + "/DEB_OUT.deb"}, {DISTRO_DIR + "/DEB_OUT.deb.md5"},
                                    {DISTRO_DIR + "/ZIP_OUT.zip"}, {DISTRO_DIR + "/ZIP_OUT.zip.md5"}});

    // Run deploy installer
    runTestParams({"-bin", bin, "clear",
                   "qif", "-qifOut", "QIF_OUT.exe",
                   "deb", "-debOut", "DEB_OUT.deb",
                   "zip", "-zipOut", "ZIP_OUT.zip"}, &result);
#else
    auto result = utils.createTree({{DISTRO_DIR + "/QIF_OUT.exe"}, {DISTRO_DIR + "/QIF_OUT.exe.md5"},
                                    {DISTRO_DIR + "/ZIP_OUT.zip"}, {DISTRO_DIR + "/ZIP_OUT.zip.md5"}});

    // Run deploy installer
    runTestParams({"-bin", bin, "clear",
                   "qif", "-qifOut", "QIF_OUT.exe",
                   "zip", "-zipOut", "ZIP_OUT.zip"}, &result);
#endif

}
