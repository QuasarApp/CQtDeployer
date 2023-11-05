//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "releativelinktest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pathutils.h>
#include <pluginsparser.h>


void ReleativeLinkTest::test() {
    auto cases = QList<QList<QString>>{
    {"", "", "./"},
    {"/media", "/etc", "./../etc/"},
    {"/media///", "/etc///", "./../etc/"},
    {"/media/etc/usr", "/media/etc", "./../"},
    {"/media/etc", "/media/etc/usr", "./usr/"},

    {"C:/", "C:/", "./"},
    {"C:\\", "C:/", "./"},
    {"C:/", "C:\\", "./"},

    {"C:/media", "C:/etc", "./../etc/"},
    {"C:/media//\\", "C:/etc///", "./../etc/"},
    {"C:/media/etc/usr", "C:/media/etc", "./../"},
    {"C:/media\\etc", "C:/media/etc/usr", "./usr/"},
    {"C:/media/etc", "D:/media/etc/usr", "D:/media/etc/usr"},

};

    for (const auto &i: std::as_const(cases)) {
        if (PathUtils::getRelativeLink(i[0], i[1]) != i[2])
            QVERIFY(false);
    }

    for (int i = 1; i < cases.size() - 1; i++) {
        if (!PathUtils::isAbsalutPath(cases[i][0]))
            QVERIFY(false);
        if (PathUtils::isAbsalutPath(cases[i][2]))
            QVERIFY(false);

    }

}
