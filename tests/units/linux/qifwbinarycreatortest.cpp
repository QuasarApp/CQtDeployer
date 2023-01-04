//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "qifwbinarycreatortest.h"
#include "Distributions/qif.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void QIFWBinaryCreatorTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = {TestBinDir + "TestOnlyC"};
#else
    QString bin = {TestBinDir + "TestOnlyC.exe"};
#endif

    {
        QuasarAppUtils::Params::parseParams({
                                                "-bin", bin,
                                                "qifFromSystem",
                                                "clear",
                                                "qif",
                                                "-binarycreator", "test testValue"
                                            });
        Deploy deploy;
        QVERIFY(deploy.prepare());
        FileManager fm;
        QIF qif(&fm);

        auto command = qif.runCmd();
        QVERIFY(command.size() == 1);
        QVERIFY(command.first().command == "test");
        QVERIFY(command.first().arguments.contains("testValue"));
    }

    {
        QuasarAppUtils::Params::parseParams({
                                                "-bin", bin,
                                                "qifFromSystem",
                                                "clear",
                                                "qif",
                                                "-binarycreator", "test"
                                            });
        Deploy deploy;
        QVERIFY(deploy.prepare());
        FileManager fm;
        QIF qif(&fm);

        auto command = qif.runCmd();
        QVERIFY(command.size() == 1);
        QVERIFY(command.first().command == "test");
    }

}
