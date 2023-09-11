//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "overridingtemplatedebtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void OverridingDefaultTemplateDEBTest::test() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC" + "," + TestBinDir + "TestCPPOnly";
    QString qmake = TestQtDir + "bin/qmake";

    QDir tempalteDir("temaplate");
    if (!tempalteDir.isEmpty()) {
        tempalteDir.removeRecursively();
    };

    auto comapareTree = utils.createTree(
                {
                    "temaplate/defaultDEBTemplate/Test/DEBIAN/control",
                    "temaplate/defaultDEBTemplate/Test/DEBIAN/postinst",
                    "temaplate/defaultDEBTemplate/Test/DEBIAN/prerm"
                });
    QFile appScript("temaplate/defaultDEBTemplate/Test/DEBIAN/control");
    appScript.remove();

    runTestParams(
                {"force-clear",
                 "getDefaultTemplate",
                 "-name", "Test",
                 "deb",
                 "-targetDir", "temaplate",
                 "-targetPackage", "MyApp"
                }, &comapareTree);

    comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/Test.deb",
                    "./" + DISTRO_DIR + "/Test.deb.md5",

                });

    runTestParams(
                {"force-clear",
                 "-deb", "temaplate/defaultDEBTemplate",
                 "-targetPackage", "MyApp",
                 "-name", "Test",
                 "-qmake", qmake
                }, &comapareTree, false, false);


    QVERIFY(appScript.open(QIODevice::WriteOnly));
    QVERIFY(appScript.write(QByteArray{"ERROR"}));
    appScript.close();

    runTestParams(
                {"-bin", bin,
                 "force-clear",
                 "-deb", "temaplate/defaultDEBTemplate",
                 "-targetPackage", "MyApp",
                 "-name", "Test",
                 "-qmake", qmake
                }, nullptr, false, false, exitCodes::PackingError);
#endif

}
