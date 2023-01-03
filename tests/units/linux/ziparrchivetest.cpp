//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "ziparrchivetest.h"
#include "zipcompresser.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void ZIPArchiveTest::test() {
    TestUtils utils;

    ZipCompresser zip;
    auto befor = utils.getTree("./test");

    QVERIFY(zip.compress("./test", "./arr.zip"));
    QVERIFY(QDir("./test").removeRecursively());
    QVERIFY(zip.extract("./arr.zip", "./test"));

    auto after = utils.getTree("./test");

    QVERIFY(utils.compareTree(befor, after).size() == 0);

}
