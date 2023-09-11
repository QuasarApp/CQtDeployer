//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "deploytargettest.h"

#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


DeployTargetTest::DeployTargetTest() {

}

DeployTargetTest::~DeployTargetTest() {

}


void DeployTargetTest::test() {
    FileManager file;
    DependenciesScanner scan;
    Packing pac(&file);
    PluginsParser _pluginParser;

    ConfigParser *deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);

    QStringList targets;
    targets << "./test/bins/execTarget.exe";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bins/execTarget.exe" << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bns/execTarget.exe";
    QVERIFY(!deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bins/";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bins/" << "./test/warning/";
    QVERIFY(deploy->setTargets(targets));

    delete deploy;
    targets.clear();
}
