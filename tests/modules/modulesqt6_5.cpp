/*
 * Copyright (C) 2023-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "modulesqt6_5.h"

ModulesQt6_5::ModulesQt6_5()
{

}

QSet<QString> ModulesQt6_5::qmlLibs(const QString &distDir) const {
    TestUtils utils;
    auto Tree = ModulesQt6_4::qmlLibs(distDir);

    Tree -= utils.createTree(
        {
            "./" + distDir + "/qml/QtQml/qmlplugin.dll",
            "./" + distDir + "/qml/QtQml/libqmlplugin.so",
            "./" + distDir + "/qml/QtQml/plugins.qmltypes"
        }
    );

#ifdef Q_OS_WIN
    Tree += utils.createTree(
        {
            "./" + distDir + "/qml/QtQml/Base/qmlplugin.dll",
            "./" + distDir + "/qml/QtQml/Base/plugins.qmltypes",
            "./" + distDir + "/qml/QtQml/Base/qmldir",
            "./" + distDir + "/qml/QtQml/qmlmetaplugin.dll",
            "./" + distDir + "/qml/QtQuick/Controls/Material/impl/RoundedElevationEffect.qml",
            "./" + distDir + "/qml/QtQuick/Controls/Windows/ApplicationWindow.qml"

        }
        );
#else
    Tree += utils.createTree(
        {
            "./" + distDir + "/qml/QtQml/Base/libqmlplugin.so",
            "./" + distDir + "/qml/QtQml/Base/plugins.qmltypes",
            "./" + distDir + "/qml/QtQml/Base/qmldir",
            "./" + distDir + "/qml/QtQml/libqmlmetaplugin.so",
            "./" + distDir + "/qml/QtQuick/Controls/Material/impl/RoundedElevationEffect.qml"
        }
        );
#endif

    return Tree;
}

QSet<QString> ModulesQt6_5::qmlVirtualKeyBoadrLibs(const QString &distDir) const {

    TestUtils utils;

    auto Tree = ModulesQt6_4::qmlVirtualKeyBoadrLibs(distDir);
    return Tree;
}

QSet<QString> ModulesQt6_5::qtWebEngine(const QString &distDir) const
{
    TestUtils utils;

    auto Tree = ModulesQt6_4::qtWebEngine(distDir);
    return Tree;
}

QSet<QString> ModulesQt6_5::qtLibs(const QString &distDir) const {
    auto Tree = ModulesQt6_4::qtLibs(distDir);
    TestUtils utils;

#ifdef Q_OS_WIN
    Tree += utils.createTree(
        {
            "./" + distDir + "/plugins/platforms/qdirect2d.dll",
        }
        );
#endif

    return Tree;
}

QSet<QString> ModulesQt6_5::qtWebEngineWidgets(const QString &distDir) const
{
    TestUtils utils;

    auto Tree = ModulesQt6_4::qtWebEngineWidgets(distDir);

    return Tree;
}
