//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "qmlextracttest.h"
#include "qmlqt5.h"
#include "qmlcreator.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void QMLExtractTest::test() {
    QmlCreator creator("./");
    auto imports = creator.getQmlImports();

    auto qmlFiles = creator.getCopyedQml();


    QMLQt5 scaner("./");


    for (const auto &file : std::as_const(qmlFiles)) {


        auto fileImports = scaner.extractImportsFromFile(file);

        for (const auto &fil : imports.value(file)) {
            QVERIFY(fileImports.contains(fil, Qt::CaseInsensitive));
        }

    }
}
