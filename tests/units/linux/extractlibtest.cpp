//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "extractlibtest.h"
#include "libcreator.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


ExtractLibTest::ExtractLibTest() {

}

ExtractLibTest::~ExtractLibTest() {

}

void ExtractLibTest::test() {
    LibCreator creator("./");
    auto libs = creator.getLibs();
    auto deb = creator.getLibsDep();
    auto platforms = creator.getLibplatform();

    DependenciesScanner scaner;

    LibInfo info;

    for (const auto &lib : libs) {
        QVERIFY(scaner.fillLibInfo(info, lib));
        QVERIFY(info.getName() == QFileInfo(lib).fileName());
        QVERIFY(info.getPath() == QFileInfo(lib).absolutePath());
        QVERIFY(info.fullPath() == QFileInfo(lib).absoluteFilePath());
        QVERIFY(info.getPlatform() == platforms.value(lib));

        for (const auto &dep : deb.value(lib)) {
            QString depName = dep;
            if (info.getPlatform() & Platform::Win) {
                depName = dep.toUpper();
            }

            bool test = info.getDependncies().contains(depName);
            QVERIFY(test);
        }

    }

}
