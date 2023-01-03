//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "qifwouttest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void QIFWOutTest::test() {
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
