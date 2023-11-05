//#
//# Copyright (C) 2020-2023 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#include "extrapluginstest.h"
#include "modules.h"
#include <configparser.h>
#include <dependenciesscanner.h>
#include <filemanager.h>
#include <packing.h>
#include <pluginsparser.h>


void ExtraPluginTest::test() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "QtWidgetsProject";
    QString qmake = TestQtDir + "bin/qmake";

    auto pluginTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlodbc.so",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlpsql.so",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlite.so",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlmysql.so",
                    "./" + DISTRO_DIR + "/lib/libQt6Sql.so",
                    "./" + DISTRO_DIR + "/lib/libpq.so",
                    "./" + DISTRO_DIR + "/lib/libcrypto.so",
                    "./" + DISTRO_DIR + "/lib/libssl.so",
                    "./" + DISTRO_DIR + "/lib/libmysqlclient.so",


                });

#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    pluginTree += utils.createTree({"./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlmimer.so"});
#endif

#else
    QString bin = TestBinDir + "QtWidgetsProject.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

    auto pluginTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlmysql.dll",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlodbc.dll",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlite.dll",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlpsql.dll",
                    "./" + DISTRO_DIR + "/Qt6Sql.dll",
                    "./" + DISTRO_DIR + "/libpq.dll",

                });

#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    pluginTree.insert("./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlmimer.dll");
#endif
#endif

    auto comapareTree = TestModule.qtLibs();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                  }, &comapareTree);


    comapareTree = comapareTree + pluginTree;


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-enablePlugins", "sqldrivers"}, &comapareTree);

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-extraPlugin", TestQtDir + "/plugins/sqldrivers"}, &comapareTree);

    comapareTree -= pluginTree;
    comapareTree -= utils.createTree(
                {
                    "./" + DISTRO_DIR + "/plugins/platforms/libqxcb.so",
                    "./" + DISTRO_DIR + "/lib/libxcb-xinerama.so.0",
                    "./" + DISTRO_DIR + "/plugins/xcbglintegrations/libqxcb-egl-integration.so",
                    "./" + DISTRO_DIR + "/plugins/xcbglintegrations/libqxcb-glx-integration.so",
                    "./" + DISTRO_DIR + "/lib/libQt6XcbQpa.so",

                });

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-disablePlugins", "qxcb,xcbglintegrations"}, &comapareTree);

}
