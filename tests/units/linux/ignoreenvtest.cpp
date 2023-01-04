//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "ignoreenvtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void IgnoreEnvTest::test() {



    Envirement env;
    QDir("./testTree").removeRecursively();

    QStringList ignoreTree = {
        "./testTree/test",
        "./testTree/",
        "./testTree/test1/1",
        "./testTree/test2/1/",
    };

    QStringList testTree = {
        "./testTree/test/s",
        "./testTree/s",
        "./testTree/test1/1s",
        "./testTree/test2/1/s",
    };

    deployTree(ignoreTree);
    deployTree(testTree);

    env.setIgnoreEnvList(ignoreTree);
    env.addEnv(ignoreTree);

    // must be empty becouse all pathes is ignored
    QVERIFY(env.size() == 0);

    env.addEnv(testTree);

    // must be equals 4 becouse all pathes is not ignored
    QVERIFY(env.size() == 4);

    // try add dublicate
    env.addEnv(testTree);

    // must be equals 4 becouse all dublicates must be ignored
    QVERIFY(env.size() == 4);

    QVERIFY(QDir("./testTree").removeRecursively());
}
