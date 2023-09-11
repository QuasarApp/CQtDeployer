//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "confifiletest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>
#include "modules.h"


void ConfFileTest::test() {
     TestUtils utils;


    QFile::remove(TestBinDir + "/TestConf.json");
    QFile::remove(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");

#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/bin/TestOnlyC",
                 "./" + DISTRO_DIR + "/bin/qt.conf",
                 "./" + DISTRO_DIR + "/TestOnlyC.sh"});
#else
    auto comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "/qt.conf"});
#endif

#ifdef Q_OS_UNIX
    runTestParams({"-bin", TestBinDir + "TestOnlyC", "clear" , "noCheckRPATH", "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#else
    runTestParams({"-bin", TestBinDir + "TestOnlyC.exe", "clear" , "noCheckRPATH", "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#endif


    QVERIFY(QFile::exists(TestBinDir + "/TestConf.json"));
    QFile::remove(TestBinDir + "/TestConf.json");



#ifdef Q_OS_UNIX
    comapareTree += utils.createTree(
                {"./" + DISTRO_DIR + "/bin/TestCPPOnly",
                 "./" + DISTRO_DIR + "/TestCPPOnly.sh"});
#else
    comapareTree += utils.createTree(
                {"./" + DISTRO_DIR + "/TestCPPOnly.exe",
                 "./" + DISTRO_DIR + "/TestCPPOnly.bat"});
#endif

#ifdef Q_OS_UNIX
    runTestParams({"-bin", TestBinDir + "TestOnlyC," + TestBinDir + "TestCPPOnly",
                   "clear", "noCheckRPATH", "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#else
    runTestParams({"-bin", TestBinDir + "TestOnlyC.exe," + TestBinDir + "TestCPPOnly.exe",
                   "clear" , "-libDir", "L:/never/absalut/path", "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#endif

    QFile confFile(TestBinDir + "/TestConf.json");
    QVERIFY(confFile.open(QIODevice::ReadOnly));

    auto data = confFile.readAll();
    confFile.close();

    QJsonDocument doc;
    doc = doc.fromJson(data);
    QVERIFY(!doc.isNull());

#ifdef Q_OS_UNIX

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("./TestOnlyC"));
    QVERIFY(data.contains("./TestCPPOnly"));

    QVERIFY(data.contains("\"clear\": true"));

    data.insert(data.size() - 2, QString(",\"libDir\": \"/never/absalut/path/\"").toLatin1());

    QVERIFY(confFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    confFile.write(data);
    confFile.close();

    runTestParams({"-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#else

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("./TestOnlyC.exe"));
    QVERIFY(data.contains("./TestCPPOnly.exe"));
    QVERIFY(data.contains("\"libDir\": \"L:/never/absalut/path\""));

    QVERIFY(data.contains("\"clear\": true"));

    runTestParams({"-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);

#endif

    QVERIFY(QuasarAppUtils::Params::isEndable("clear"));
    QVERIFY(QuasarAppUtils::Params::isEndable("bin"));
    QVERIFY(QuasarAppUtils::Params::isEndable("libDir"));
#ifdef Q_OS_UNIX
    QVERIFY(QuasarAppUtils::Params::getArg("libDir") == "/never/absalut/path/");
#else
    QVERIFY(QuasarAppUtils::Params::getArg("libDir") == "L:/never/absalut/path");
#endif
    QFile::remove(TestBinDir + "/TestConf.json");


#ifdef Q_OS_UNIX
    runTestParams({"-bin", TestBinDir + "TestOnlyC," + TestBinDir + "TestCPPOnly",
                   "clear" , "noCheckRPATH", "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);
#else
    runTestParams({"-bin", TestBinDir + "TestOnlyC.exe," + TestBinDir + "TestCPPOnly.exe",
                   "clear" , "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);
#endif

    confFile.setFileName(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");
    QVERIFY(confFile.open(QIODevice::ReadOnly));

    data = confFile.readAll();
    confFile.close();

    doc = doc.fromJson(data);
    QVERIFY(!doc.isNull());

#ifdef Q_OS_UNIX

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("./../../../../../bin/TestOnlyC"));
    QVERIFY(data.contains("./../../../../../bin/TestCPPOnly"));

    QVERIFY(data.contains("\"clear\": true"));
    QString qmake = TestQtDir + "bin/qmake";

#else

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("./../../../../../build/TestOnlyC.exe"));
    QVERIFY(data.contains("./../../../../../build/TestCPPOnly.exe"));

    QVERIFY(data.contains("\"clear\": true"));
    QString qmake = TestQtDir + "bin/qmake.exe";

#endif
    runTestParams({"-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);

    QVERIFY(QuasarAppUtils::Params::isEndable("clear"));
    QVERIFY(QuasarAppUtils::Params::isEndable("bin"));

    QFile::remove(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");

    // Test generar string in confFile
    comapareTree = TestModule.qtLibs();

    comapareTree = TestModule.ignoreFilter(comapareTree, "/plugins/p");
#ifdef Q_OS_UNIX
    comapareTree -= utils.createTree(
                {
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
                    "./" + DISTRO_DIR + "/lib/libQt6EglFSDeviceIntegration.so",
                    "./" + DISTRO_DIR + "/lib/libQt6WaylandEglClientHwIntegration.so",
#else
                    "./" + DISTRO_DIR + "/lib/libQt5EglFSDeviceIntegration.so",
#endif

                });
    auto bin =  TestBinDir + "QtWidgetsProject";
#else
    comapareTree -= utils.createTree(
                {
                    "./" + DISTRO_DIR + "/Qt5DBus.dll"

                });
    auto bin =  TestBinDir + "QtWidgetsProject.exe";
#endif

    runTestParams({"-bin", bin,
                   "-qmake", qmake,
                   "clear",
                   "-ignore", "/plugins/p",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);


    runTestParams({"-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
    QFile::remove(TestBinDir + "/TestConf.json");

#ifdef Q_OS_UNIX
    QFile f("./" + DISTRO_DIR + "/bin/TestOnlyC");
    comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/package/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/package/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/package/bin/qt.conf"
                });
    QString target1 = TestBinDir + "TestOnlyC";

#else
    QFile f("./" + DISTRO_DIR + "/TestOnlyC.exe");
    comapareTree = utils.createTree(
                {"./" + DISTRO_DIR + "/package/TestOnlyC.exe",
                 "./" + DISTRO_DIR + "/package/TestOnlyC.bat",
                 "./" + DISTRO_DIR + "/package/qt.conf"});
    QString target1 = TestBinDir + "TestOnlyC.exe";

#endif
    bin = target1;

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "package;TestOn",
                   "-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);

    runTestParams({"-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"},
                  &comapareTree);
    QFile::remove(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");

    auto file = "testCase.json";
#ifdef Q_OS_UNIX
    bin =  TestBinDir + "QtWidgetsProject," + TestBinDir + "TestOnlyC";
#else
    bin =  TestBinDir + "QtWidgetsProject.exe," + TestBinDir + "TestOnlyC.exe";
#endif
    QVERIFY(utils.deployFile(":/testResurces/testRes/testMultiPackageConfig.json", file,
                             {{"$BIN_DIR", bin.toLatin1()}}));

    comapareTree = TestModule.onlyC(DISTRO_DIR + "/Dstro1") +
            TestModule.qtLibs(DISTRO_DIR + "/Dstro2");


    runTestParams({"-confFile", file},
                  &comapareTree);

    QFile::remove(TestBinDir + "/TestConf.json");
    QFile::remove(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");


}
