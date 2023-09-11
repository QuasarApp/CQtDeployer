//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "overridingtemplateqifwtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void OverridingDefaultTemplateQIFWTest::test() {
    TestUtils utils;
    // Prepare bin and qmake values
#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC" + "," + TestBinDir + "TestCPPOnly";
    QString qmake = TestQtDir + "bin/qmake";

#else
    QString bin = TestBinDir + "TestOnlyC.exe" + "," + TestBinDir + "TestCPPOnly.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

#endif
    QDir tempalteDir("temaplate");
    if (!tempalteDir.isEmpty()) {
        tempalteDir.removeRecursively();
    };

    // QIF case. prepare default template.
    auto comapareTree = utils.createTree(
                {
                    "temaplate/defaultQIFWTemplate/config/config.xml",
                    "temaplate/defaultQIFWTemplate/config/controlScript.qs",
                    "temaplate/defaultQIFWTemplate/packages/MyApp/meta/installscript.qs",
                    "temaplate/defaultQIFWTemplate/packages/MyApp/meta/package.xml"
                });
    QFile appScript("temaplate/defaultQIFWTemplate/packages/MyApp/meta/installscript.qs");
    appScript.remove();

    // This command shold be deploy default template in the template folder
    runTestParams(
                {"-bin", bin,
                 "force-clear",
                 "getDefaultTemplate",
                 "-name", "Test",
                 "qif",
                 "-targetDir", "temaplate",
                 "-targetPackage", "MyApp"
                }, &comapareTree);

#ifdef Q_OS_UNIX
    comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/InstallerTest.run",
                    "./" + DISTRO_DIR + "/InstallerTest.run.md5",
                });

#else
    comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/InstallerTest.exe",
                    "./" + DISTRO_DIR + "/InstallerTest.exe.md5",
                });

#endif

    // check deploy application with custom template
    runTestParams(
                {"-bin", bin,
                 "force-clear",
                 "-qif", "temaplate/defaultQIFWTemplate",
                 "-targetPackage", "MyApp",
                 "-name", "Test",
                 "-qmake", qmake,
                 "qifFromSystem"
                }, &comapareTree, false, false);

    QVERIFY(appScript.open(QIODevice::WriteOnly));
    QVERIFY(appScript.write(QByteArray{"ERROR"}));
    appScript.close();

    // Shold be failde because we added error string into template files.
    runTestParams(
                {"-bin", bin,
                 "force-clear",
                 "-qif", "temaplate/defaultQIFWTemplate",
                 "-targetPackage", "MyApp",
                 "-name", "Test",
                 "-qmake", qmake,
                 "qifFromSystem"
                }, nullptr, false, false, exitCodes::PackingError);



}
