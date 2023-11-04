//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "distrostructtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void DistroStructTest::test() {
    DistroStruct distro;

    auto cases = QList<QPair<QString,QString>>{
        {"", "/"},
        {"/", "/"},
        {"/res","/../"},
        {"/res/","/../"},
        {"/res/type","/../../"},
        {"/res/type/","/../../"},
        {"res/type","../../"},
        {"res/type/","../../"},
        {"res//type/","../../"},
        {"res////type/","../../"},
        {"//res///type/////","/../../"},
        {"\\", "/"},
        {"\\res","/../"},
        {"\\res\\","/../"},
        {"\\res\\type","/../../"},
        {"\\res\\type\\","/../../"},
        {"res\\type","../../"},
        {"res\\type\\","../../"},
        {"res\\\\type\\","../../"},
        {"res\\\\\\\\type\\","../../"},
        {"\\\\res\\\\\\type\\\\\\\\\\","/../../"},
    };

    for (const auto &i: std::as_const(cases)) {
        if (distro.getRelativePath(i.first) != i.second)
            QVERIFY(false);
    }

    distro = DistroStruct();

    distro.setTrOutDir("/tr/");
    QVERIFY(distro.getTrOutDir() == "/tr/");


    distro.setTrOutDir("/tr");
    QVERIFY(distro.getTrOutDir() == "/tr/");

    distro.setTrOutDir("tr");
    QVERIFY(distro.getTrOutDir() == "/tr/");


}
