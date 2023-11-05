/*
 * Copyright (C) 2023-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "modulesqt6_6.h"

ModulesQt6_6::ModulesQt6_6()
{

}

QSet<QString> ModulesQt6_6::qmlLibs(const QString &distDir) const {
    TestUtils utils;
    auto Tree = ModulesQt6_5::qmlLibs(distDir);

#ifdef Q_OS_WIN

#else

#endif

    return Tree;
}

QSet<QString> ModulesQt6_6::qmlVirtualKeyBoadrLibs(const QString &distDir) const {

    TestUtils utils;

    auto Tree = ModulesQt6_5::qmlVirtualKeyBoadrLibs(distDir);
    return Tree;
}

QSet<QString> ModulesQt6_6::qtWebEngine(const QString &distDir) const
{
    TestUtils utils;

    auto Tree = ModulesQt6_5::qtWebEngine(distDir);

#ifdef Q_OS_WIN
    Tree += utils.createTree(
        {
            "./" + distDir + "/resources/v8_context_snapshot.bin"

        }
        );
#else
    Tree += utils.createTree(
        {
            "./" + distDir + "/resources/v8_context_snapshot.bin",
            "./" + distDir + "/lib/libQt6WebChannelQuick.so",
            "./" + distDir + "/qml/QtWebChannel/libwebchannelplugin.so",
            "./" + distDir + "/qml/QtWebChannel/libwebchannelquickplugin.so",

        }
        );
#endif

    return Tree;
}

QSet<QString> ModulesQt6_6::qtLibs(const QString &distDir) const {
    auto Tree = ModulesQt6_5::qtLibs(distDir);
    TestUtils utils;


    return Tree;
}

QSet<QString> ModulesQt6_6::qtWebEngineWidgets(const QString &distDir) const
{
    TestUtils utils;

    auto Tree = ModulesQt6_5::qtWebEngineWidgets(distDir);
#ifdef Q_OS_WIN
    Tree += utils.createTree(
        {
            "./" + distDir + "/resources/v8_context_snapshot.bin"

        }
        );
#else
    Tree += utils.createTree(
        {
            "./" + distDir + "/resources/v8_context_snapshot.bin"
        }
        );
#endif
    return Tree;
}
