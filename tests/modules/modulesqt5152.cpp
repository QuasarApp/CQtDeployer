/*
 * Copyright (C) 2023-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "modulesqt5152.h"

ModulesQt5152::ModulesQt5152()
{

}

QSet<QString> ModulesQt5152::qmlLibs(const QString &distDir) const {
    TestUtils utils;

    auto res = ModulesQt515::qmlLibs(distDir);
#ifdef Q_OS_WIN
    res += utils.createTree({
                                "./" + distDir + "/qml/QtQml/WorkerScript.2/qmldir",
                                "./" + distDir + "/qml/QtQml/qmldir",
                                "./" + distDir + "/qml/QtQml/qmlplugin.dll",
                                "./" + distDir + "/qml/QtQml/RemoteObjects/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/RemoteObjects/qtqmlremoteobjects.dll",
                                "./" + distDir + "/qml/QtQml/StateMachine/qtqmlstatemachine.dll",
                                "./" + distDir + "/qml/QtQml/Models.2/modelsplugin.dll",
                                "./" + distDir + "/Qt5RemoteObjects.dll",
                                "./" + distDir + "/qml/QtQml/Models.2/qmldir",
                                "./" + distDir + "/qml/QtQml/WorkerScript.2/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/RemoteObjects/qmldir",
                                "./" + distDir + "/qml/QtQml/Models.2/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/StateMachine/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/WorkerScript.2/workerscriptplugin.dll",
                                "./" + distDir + "/qml/QtQml/StateMachine/qmldir",

    });
#else
    res += utils.createTree({
                                "./" + distDir + "/qml/QtQml/WorkerScript.2/qmldir",
                                "./" + distDir + "/qml/QtQml/qmldir",
                                "./" + distDir + "/qml/QtQml/libqmlplugin.so",
                                "./" + distDir + "/qml/QtQml/RemoteObjects/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/RemoteObjects/libqtqmlremoteobjects.so",
                                "./" + distDir + "/qml/QtQml/StateMachine/libqtqmlstatemachine.so",
                                "./" + distDir + "/qml/QtQml/Models.2/libmodelsplugin.so",
                                "./" + distDir + "/lib/libQt5RemoteObjects.so",
                                "./" + distDir + "/qml/QtQml/Models.2/qmldir",
                                "./" + distDir + "/qml/QtQml/WorkerScript.2/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/RemoteObjects/qmldir",
                                "./" + distDir + "/qml/QtQml/Models.2/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/StateMachine/plugins.qmltypes",
                                "./" + distDir + "/qml/QtQml/WorkerScript.2/libworkerscriptplugin.so",
                                "./" + distDir + "/qml/QtQml/StateMachine/qmldir",


    });
#endif

    return res;
}

QSet<QString> ModulesQt5152::testDistroLibs(const QString &distDir) const {
    TestUtils utils;

    auto res = ModulesQt515::testDistroLibs(distDir);

#ifdef Q_OS_WIN

    res += utils.createTree({
                                "./" + distDir + "/package2/ZzZ/Qt5RemoteObjects.dll",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/Models.2/modelsplugin.dll",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/Models.2/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/Models.2/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/RemoteObjects/qtqmlremoteobjects.dll",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/RemoteObjects/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/RemoteObjects/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/StateMachine/qtqmlstatemachine.dll",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/StateMachine/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/StateMachine/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/WorkerScript.2/workerscriptplugin.dll",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/WorkerScript.2/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/WorkerScript.2/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/qmlplugin.dll",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/qmldir",

    });

#else
    res += utils.createTree({

                                "./" + distDir + "/package2/ZzZ/lib/libQt5RemoteObjects.so",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/Models.2/libmodelsplugin.so",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/Models.2/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/Models.2/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/RemoteObjects/libqtqmlremoteobjects.so",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/RemoteObjects/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/RemoteObjects/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/StateMachine/libqtqmlstatemachine.so",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/StateMachine/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/StateMachine/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/WorkerScript.2/libworkerscriptplugin.so",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/WorkerScript.2/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/WorkerScript.2/qmldir",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/libqmlplugin.so",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/plugins.qmltypes",
                                "./" + distDir + "/package2/ZzZ/q/and/q/QtQml/qmldir",

                            });


#endif
    return res;
}

QSet<QString> ModulesQt5152::testOutLibs(const QString &distDir) const {
    TestUtils utils;

    auto res = ModulesQt515::testOutLibs(distDir);

#ifdef Q_OS_WIN
    res += utils.createTree({
                                "./" + distDir + "/lolLib/Qt5RemoteObjects.dll",
                                "./" + distDir + "/q/QtQml/Models.2/modelsplugin.dll",
                                "./" + distDir + "/q/QtQml/Models.2/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/Models.2/qmldir",
                                "./" + distDir + "/q/QtQml/RemoteObjects/qtqmlremoteobjects.dll",
                                "./" + distDir + "/q/QtQml/RemoteObjects/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/RemoteObjects/qmldir",
                                "./" + distDir + "/q/QtQml/StateMachine/qtqmlstatemachine.dll",
                                "./" + distDir + "/q/QtQml/StateMachine/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/StateMachine/qmldir",
                                "./" + distDir + "/q/QtQml/WorkerScript.2/workerscriptplugin.dll",
                                "./" + distDir + "/q/QtQml/WorkerScript.2/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/WorkerScript.2/qmldir",
                                "./" + distDir + "/q/QtQml/qmlplugin.dll",
                                "./" + distDir + "/q/QtQml/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/qmldir",


    });
#else
    res += utils.createTree({
                                "./" + distDir + "/lolLib/libQt5RemoteObjects.so",
                                "./" + distDir + "/q/QtQml/Models.2/libmodelsplugin.so",
                                "./" + distDir + "/q/QtQml/Models.2/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/Models.2/qmldir",
                                "./" + distDir + "/q/QtQml/RemoteObjects/libqtqmlremoteobjects.so",
                                "./" + distDir + "/q/QtQml/RemoteObjects/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/RemoteObjects/qmldir",
                                "./" + distDir + "/q/QtQml/StateMachine/libqtqmlstatemachine.so",
                                "./" + distDir + "/q/QtQml/StateMachine/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/StateMachine/qmldir",
                                "./" + distDir + "/q/QtQml/WorkerScript.2/libworkerscriptplugin.so",
                                "./" + distDir + "/q/QtQml/WorkerScript.2/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/WorkerScript.2/qmldir",
                                "./" + distDir + "/q/QtQml/libqmlplugin.so",
                                "./" + distDir + "/q/QtQml/plugins.qmltypes",
                                "./" + distDir + "/q/QtQml/qmldir",
                            });

#endif
    return res;


}
