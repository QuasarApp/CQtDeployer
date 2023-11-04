//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "iconstest.h"
#include "defines.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void IconsTest::test() {
   TestUtils utils;

    auto initTargets = [](ConfigParser * deploy, const QStringList& params) {
        QVERIFY(QuasarAppUtils::Params::parseParams(params));

        auto bin = QuasarAppUtils::Params::getArg("bin").
                split(DeployCore::getSeparator(0), splitbehavior);


        QVERIFY(deploy->setTargets(bin));

        deploy->configureTargets();
    };

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    QStringList binMulti = {TestBinDir + "TestOnlyC" , TestBinDir + "TestCPPOnly"};

#else
    QString bin = TestBinDir + "TestOnlyC.exe";
    QStringList binMulti = {TestBinDir + "TestOnlyC.exe" , TestBinDir + "TestCPPOnly.exe"};

#endif

    FileManager file;
    DependenciesScanner scan;
    Packing pac(&file);
    PluginsParser _pluginParser;


    // Test default icons
    ConfigParser *deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);

    initTargets(deploy, {"-bin", bin,
                         "force-clear",
                });

    auto targetsMap = deploy->_config.getTargetsListByFilter("TestOnlyC");
    QVERIFY(targetsMap.size());

    auto targetinfo = targetsMap.begin().value();
    QVERIFY(targetinfo->getName().contains("TestOnlyC"));
    auto icon = QFileInfo(targetinfo->getIcon());

#ifdef Q_OS_UNIX
    QVERIFY(icon.fileName() == "Icon.png");
#else
    QVERIFY(icon.fileName() == "Icon.ico");

#endif
    delete deploy;

    // Test multiple targets icons (sets for all targets on icon)
    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);

    initTargets(deploy, {"-bin", binMulti.join(","),
                         "force-clear",
                         "-icon", ":/testResurces/testRes/TestIcon.png",
                });

    for (const auto &target : std::as_const(binMulti)) {
        QFileInfo tarInfo(target);

        targetsMap = deploy->_config.getTargetsListByFilter(tarInfo.baseName());
        targetinfo = targetsMap.begin().value();
        QVERIFY(targetinfo->getName().contains(tarInfo.baseName()));
        icon = QFileInfo(targetinfo->getIcon());

        QVERIFY(icon.fileName() == "TestIcon.png");
    }

    delete deploy;


    // Test multiple targets icons (sets only one icon)
    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);

    initTargets(deploy, {"-bin", binMulti.join(","),
                         "force-clear",
                         "-icon", "TestOnlyC;:/testResurces/testRes/TestIcon.png",
                });


    targetsMap = deploy->_config.getTargetsListByFilter("TestOnlyC");
    targetinfo = targetsMap.begin().value();
    QVERIFY(targetinfo->getName().contains("TestOnlyC"));
    icon = QFileInfo(targetinfo->getIcon());

    QVERIFY(icon.fileName() == "TestIcon.png");


    delete deploy;

    // manual tests
#ifdef QT_DEBUG
#ifdef Q_OS_UNIX
    bin = TestBinDir + "TestOnlyC," + TestBinDir + "TestCPPOnly";

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/InstallerTest.run",
                                             "./" + DISTRO_DIR + "/InstallerTest.run.md5",
                                         });

#else
    bin = TestBinDir + "TestOnlyC.exe,"  + TestBinDir + "TestCPPOnly.exe";

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/InstallerTest.exe",
                                             "./" + DISTRO_DIR + "/InstallerTest.exe.md5",

                                         });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "qif", "qifFromSystem", "-name", "Test",
                   "-targetPackage", "testPackage;TestOnlyC;TestCPPOnly",
                   "-icon", "TestOnlyC;:/testResurces/testRes/TestIcon.png,TestCPPOnly;:/testResurces/testRes/TestTr.qm"}, &comapareTree, true);

#endif
}
