//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "dependencymaptest.h"
#include "dependencymap.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void DependencyMapTest::test() {
    DependencyMap dep1, dep2, dep3;

    QVERIFY(dep1.qtModules() == 0);
    QVERIFY(dep2.qtModules() == 0);
    QVERIFY(dep3.qtModules() == 0);

    QVERIFY(dep1.systemLibs().isEmpty());
    QVERIFY(dep2.systemLibs().isEmpty());
    QVERIFY(dep3.systemLibs().isEmpty());


    QVERIFY(dep1.neadedLibs().isEmpty());
    QVERIFY(dep2.neadedLibs().isEmpty());
    QVERIFY(dep3.neadedLibs().isEmpty());

    dep1.addModule(DeployCore::QtModule::QtGuiModule);

    QVERIFY(dep1.qtModules() == DeployCore::QtModule::QtGuiModule);
    dep1.addModule(DeployCore::QtModule::QtHelpModule);

    QVERIFY(dep1.qtModules() == (DeployCore::QtModule::QtGuiModule |
                                 DeployCore::QtModule::QtHelpModule));

    dep1.removeModule(DeployCore::QtModule::QtGuiModule);

    QVERIFY(dep1.qtModules() == DeployCore::QtModule::QtHelpModule);

    dep2.addModule(DeployCore::QtModule::QtGuiModule);

    dep1 += dep2;

    QVERIFY(dep1.qtModules() == (DeployCore::QtModule::QtGuiModule |
                                 DeployCore::QtModule::QtHelpModule));


}
