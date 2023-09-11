/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "modulesqt514.h"
#include <configparser.h>

ModulesQt514::ModulesQt514()
{

}

QSet<QString> ModulesQt514::qtLibs(const QString &distDir) const
{
    TestUtils utils;
    auto res = ModulesQt513::qtLibs(distDir);

#ifdef Q_OS_WIN
    res -= utils.createTree({
    });
#else
    res += utils.createTree({
    });
#endif

    return res;
}

QSet<QString> ModulesQt514::onlyC(const QString &distDir) const
{
    TestUtils utils;
    auto res = ModulesQt513::onlyC(distDir);
    return res;
}

QSet<QString> ModulesQt514::qmlLibs(const QString &distDir) const
{
    TestUtils utils;

    auto res = ModulesQt513::qmlLibs(distDir);

#ifdef Q_OS_WIN
    res += utils.createTree({
        {"./" + distDir + "/Qt5QmlModels.dll"},
        {"./" + distDir + "/Qt5QmlWorkerScript.dll"},
    });
#else
    res += utils.createTree({
        {"./" + distDir + "/lib/libQt5QmlModels.so"},
        {"./" + distDir + "/lib/libQt5QmlWorkerScript.so"},
        {"./" + distDir + "/plugins/wayland-graphics-integration-client/libvulkan-server.so"},
        {"./" + distDir + "/plugins/wayland-graphics-integration-server/libvulkan-server.so"},
    });
#endif

    return res;
}

QSet<QString> ModulesQt514::qtWithoutTr(const QString &distDir) const
{
    TestUtils utils;
    auto res = ModulesQt513::qtWithoutTr(distDir);

#ifdef Q_OS_WIN
    res += utils.createTree({
    });
#else
    res += utils.createTree({
    });
#endif
    return res;
}

QSet<QString> ModulesQt514::qtWebEngine(const QString &distDir) const
{
    TestUtils utils;

    auto res = ModulesQt513::qtWebEngine(distDir);

#ifdef Q_OS_WIN
#else
    res += utils.createTree({
        "./" + distDir + "/lib/libQt5QmlModels.so",
        "./" + distDir + "/lib/libQt5QmlWorkerScript.so",
        "./" + distDir + "/qml/QtQml/WorkerScript.2/libworkerscriptplugin.so",
        "./" + distDir + "/qml/QtQml/WorkerScript.2/plugins.qmltypes",
        "./" + distDir + "/qml/QtQml/WorkerScript.2/qmldir",
        "./" + distDir + "/qml/QtQml/libqmlplugin.so",
        "./" + distDir + "/translations/qtwebengine_ca.qm"
    });
#endif
    return res;
}

QSet<QString> ModulesQt514::qtWebEngineWidgets(const QString &distDir) const
{
    TestUtils utils;

    auto res = ModulesQt513::qtWebEngineWidgets(distDir);

#ifdef Q_OS_WIN
#else
    res += utils.createTree({
        "./" + distDir + "/translations/qtwebengine_ca.qm"
    });
#endif
    return res;
}

