//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "settargetdirtest.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void SetTargetDirTest::test() {

    FileManager file;
    DependenciesScanner scan;
    Packing pac(&file);
    PluginsParser _plugins;
    ConfigParser  dep(&file, &_plugins, &scan, &pac);

    dep.setTargetDir();

    QVERIFY(dep.config()->getTargetDir() == QFileInfo("./" + DISTRO_DIR + "").absoluteFilePath());
    dep.setTargetDir("./ff");
    QVERIFY(dep.config()->getTargetDir() == QFileInfo("./ff").absoluteFilePath());

    QStringList argv = {"-targetDir", "./" + DISTRO_DIR + "2"};

    QuasarAppUtils::Params::parseParams(argv);

    dep.setTargetDir();
    QVERIFY(dep.config()->getTargetDir() == QFileInfo("./" + DISTRO_DIR + "2").absoluteFilePath());
    dep.setTargetDir("./ff");
    QVERIFY(dep.config()->getTargetDir() == QFileInfo("./" + DISTRO_DIR + "2").absoluteFilePath());

}
