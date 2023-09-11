//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "md5test.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void MD5Test::test() {
    // This test will check hashes

    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    auto outFile = "./" + DISTRO_DIR + "/InstallerTestOnlyC.run";

#else
    QString bin = TestBinDir + "TestOnlyC.exe";
    auto outFile = "./" + DISTRO_DIR + "/InstallerTestOnlyC.exe";

#endif

    auto comapareTreeqif = utils.createTree(
                {
                    outFile,
                    outFile + ".md5"
                });

    // Run deploy installer
    runTestParams({"-bin", bin,
                   "force-clear",
                   "qif",
                   "qifFromSystem"}, &comapareTreeqif);

    QFile file(outFile);
    QVERIFY(file.open(QIODevice::ReadOnly));
    auto binaryData = file.readAll();
    file.close();

    file.setFileName(outFile + ".md5");
    QVERIFY(file.open(QIODevice::ReadOnly));
    auto hash = file.readAll();
    file.close();

    // Compare hash sum of the installer with realy hash sum of the object.
    QVERIFY(QCryptographicHash::hash(binaryData, QCryptographicHash::Md5).toHex() == hash);


    comapareTreeqif = utils.createTree(
                {
                    outFile,
                });

    // Check noHashSum option. CQtDeployer must be skip calculating  a hash sum of an installer.
    runTestParams({"-bin", bin,
                   "force-clear",
                   "noHashSum",
                   "qif",
                   "qifFromSystem"}, &comapareTreeqif);
}
