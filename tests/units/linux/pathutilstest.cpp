//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "pathutilstest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pathutils.h>
#include <pluginsparser.h>


void PathUtilsTest::test() {
   
    // test getName
    QMap<QString, QString> cases = {
        {"",""},
        {"test","test"},
        {"t","t"},
        {"/","/"},
        {"/test","test"},
        {"/t","t"},
        {"/test/","test"},
        {"/t/","t"},

        {"/check/test","test"},
        {"/check/t","t"},
        {"/check/test/","test"},
        {"/check/t/","t"},

        {"C:\\","C:"},
        {"\\","/"},
        {"\\test","test"},
        {"\\t","t"},
        {"\\test\\","test"},
        {"\\t\\","t"},

    };

    for (auto it = cases.begin(); it != cases.end(); ++it) {
        if (PathUtils::getName(it.key()) != it.value())
            QVERIFY(false);
    }

    struct Result {
        QString result;
        QString newPath;
    };

    // test popItem
    QMap<QString, Result> popItemCases = {
        {"", {"", ""}},
        {"test", {"test", ""}},
        {"t", {"t", ""}},
        {"/", {"/", ""}},
        {"/test", {"test", "/"}},
        {"/t", {"t", "/"}},
        {"/test/", {"test", "/"}},
        {"/t/", {"t", "/"} },

        {"/check/test", {"test", "/check/"}},
        {"/check/t", {"t", "/check/"}},
        {"/check/test/", {"test", "/check/"}},
        {"/check/t/", {"t", "/check/"}},

        {"C:\\", {"C:", ""}},
        {"\\", {"/", ""}},
        {"\\test", {"test", "/"}},
        {"\\t", {"t", "/"}},
        {"\\test\\", {"test", "/"}},
        {"\\t\\", {"t", "/"}},

    };

    for (auto it = popItemCases.begin(); it != popItemCases.end(); ++it) {
        QString path = it.key();
        QString result = PathUtils::popItem(path);
        if (path != it.value().newPath || result != it.value().result)
            QVERIFY(false);
    }
}
