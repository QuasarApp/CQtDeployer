//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "qifwachiveformattest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void QIFWArchiveFormatTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = {TestBinDir + "TestOnlyC"};

    auto result = utils.createTree({{DISTRO_DIR + "/InstallerTestOnlyC.run"},
                                    {DISTRO_DIR + "/InstallerTestOnlyC.run.md5"}});
#else
    QString bin = {TestBinDir + "TestOnlyC.exe"};

    auto result = utils.createTree({{DISTRO_DIR + "/InstallerTestOnlyC.exe"},
                                    {DISTRO_DIR + "/InstallerTestOnlyC.exe.md5"}});
#endif


    runTestParams({
                      "-bin", bin,
                      "qifFromSystem",
                      "clear",
                      "qif",
                      "-qifArchiveFormat", "zip"
                  }, &result
                  );
}
