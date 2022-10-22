//#
//# Copyright (C) 2020-2022 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "MSVCtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


MSVCTest::MSVCTest() {

}

MSVCTest::~MSVCTest() {

}

void MSVCTest::test() {
    QString testPath = "./Qt/5.11.2/msvc2017_64/bin/";

    QDir d;
    QDir oldDir("./Qt");
    oldDir.removeRecursively();
    QVERIFY(d.mkpath(testPath));


    auto msvc = DeployCore::getMSVC(testPath);

    QVERIFY(msvc & MSVCVersion::MSVC_17);
    QVERIFY(msvc & MSVCVersion::MSVC_x64);

    QDir dir("./Qt");
    dir.removeRecursively();
}
