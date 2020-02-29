/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "modulesqt514.h"
#include <configparser.h>

ModulesQt514::ModulesQt514()
{

}

QSet<QString> ModulesQt514::qtLibs()
{
    TestUtils utils;
    auto res = ModulesQt513::qtLibs();

#ifdef Q_OS_WIN
    res += utils.createTree({
        {"./" + DISTRO_DIR + "/Qt5QmlModels.dll"},
    });
#else
    res += utils.createTree({
        {"./" + DISTRO_DIR + "/lib/libQt5QmlModels.so"},
        {"./" + DISTRO_DIR + "/plugins/wayland-graphics-integration-client/libvulkan-server.so"},
        {"./" + DISTRO_DIR + "/plugins/wayland-graphics-integration-server/libvulkan-server.so"},
    });
#endif

    return res;
}

QSet<QString> ModulesQt514::qmlLibs()
{
    TestUtils utils;

    auto res = ModulesQt513::qmlLibs();

#ifdef Q_OS_WIN
    res += utils.createTree({
        {"./" + DISTRO_DIR + "/Qt5QmlModels.dll"},
        {"./" + DISTRO_DIR + "/Qt5QmlWorkerScript.dll"},
    });
#else
    res += utils.createTree({
        {"./" + DISTRO_DIR + "/lib/libQt5QmlModels.so"},
        {"./" + DISTRO_DIR + "/lib/libQt5QmlWorkerScript.so"},
        {"./" + DISTRO_DIR + "/plugins/wayland-graphics-integration-client/libvulkan-server.so"},
        {"./" + DISTRO_DIR + "/plugins/wayland-graphics-integration-server/libvulkan-server.so"},
    });
#endif

    return res;
}

QSet<QString> ModulesQt514::qmlLibsExtractPlugins()
{
    auto res = ModulesQt513::qmlLibsExtractPlugins();
    return res;
}

QSet<QString> ModulesQt514::separetedPackageslibs()
{
    TestUtils utils;
    auto res = ModulesQt513::separetedPackageslibs();

#ifdef Q_OS_WIN
    res += utils.createTree(
            {
                "./" + DISTRO_DIR + "/lolLib/Qt5QmlModels.dll",
                "./" + DISTRO_DIR + "/package2/ZzZ/Qt5QmlModels.dll",
                "./" + DISTRO_DIR + "/package2/ZzZ/Qt5QmlWorkerScript.dll",
            }
        );
#else
    res += utils.createTree(
            {
                "./" + DISTRO_DIR + "/lolLib/libQt5QmlModels.so",
                "./" + DISTRO_DIR + "/p/wayland-graphics-integration-client/libvulkan-server.so",
                "./" + DISTRO_DIR + "/p/wayland-graphics-integration-server/libvulkan-server.so",
                "./" + DISTRO_DIR + "/package2/ZzZ/lib/libQt5QmlModels.so",
                "./" + DISTRO_DIR + "/package2/ZzZ/lib/libQt5QmlWorkerScript.so",
                "./" + DISTRO_DIR + "/package2/ZzZ/plugins/wayland-graphics-integration-client/libvulkan-server.so",
                "./" + DISTRO_DIR + "/package2/ZzZ/plugins/wayland-graphics-integration-server/libvulkan-server.so",
            }
        );
#endif

    return res;
}

QSet<QString> ModulesQt514::outTestLibs()
{
    TestUtils utils;
    auto res = ModulesQt513::outTestLibs();

#ifdef Q_OS_WIN
    res += utils.createTree({
        "./" + DISTRO_DIR + "/lolLib/Qt5QmlModels.dll",
        "./" + DISTRO_DIR + "/lolLib/Qt5QmlWorkerScript.dll",
    });
#else
    res += utils.createTree({
        "./" + DISTRO_DIR + "/lolLib/libQt5QmlModels.so",
        "./" + DISTRO_DIR + "/lolLib/libQt5QmlWorkerScript.so",
        "./" + DISTRO_DIR + "/p/wayland-graphics-integration-client/libvulkan-server.so",
        "./" + DISTRO_DIR + "/p/wayland-graphics-integration-server/libvulkan-server.so"
    });
#endif

    return res;
}

QSet<QString> ModulesQt514::qtWithoutTr()
{
    TestUtils utils;
    auto res = ModulesQt513::qtWithoutTr();

#ifdef Q_OS_WIN
    res += utils.createTree({
        {"./" + DISTRO_DIR + "/Qt5QmlModels.dll"},
    });
#else
    res += utils.createTree({
        {"./" + DISTRO_DIR + "/lib/libQt5QmlModels.so"},
        {"./" + DISTRO_DIR + "/plugins/wayland-graphics-integration-client/libvulkan-server.so"},
        {"./" + DISTRO_DIR + "/plugins/wayland-graphics-integration-server/libvulkan-server.so"},
    });
#endif
    return res;
}

QSet<QString> ModulesQt514::qtWebEngine()
{
    TestUtils utils;

    auto res = ModulesQt513::qtWebEngine();

#ifdef Q_OS_WIN
#else
    res += utils.createTree({
        "./" + DISTRO_DIR + "/lib/libQt5QmlModels.so",
        "./" + DISTRO_DIR + "/lib/libQt5QmlWorkerScript.so",
        "./" + DISTRO_DIR + "/plugins/wayland-graphics-integration-client/libvulkan-server.so",
        "./" + DISTRO_DIR + "/plugins/wayland-graphics-integration-server/libvulkan-server.so",
        "./" + DISTRO_DIR + "/qml/QtQml/WorkerScript.2/libworkerscriptplugin.so",
        "./" + DISTRO_DIR + "/qml/QtQml/WorkerScript.2/plugins.qmltypes",
        "./" + DISTRO_DIR + "/qml/QtQml/WorkerScript.2/qmldir",
        "./" + DISTRO_DIR + "/qml/QtQml/libqmlplugin.so",
        "./" + DISTRO_DIR + "/translations/qtwebengine_ca.qm"
    });
#endif
    return res;
}

