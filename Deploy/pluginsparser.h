//#
//# Copyright (C) 2018-2019 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef QTMODULES_H
#define QTMODULES_H

#include <QStringList>
#include "deploy_global.h"
#include "deploycore.h"

class DependenciesScanner;

struct DEPLOYSHARED_EXPORT PluginModuleMapping
{
    const char *directoryName;
    quint64 module;
};

class DEPLOYSHARED_EXPORT PluginsParser
{
private:
    DependenciesScanner *_libScaner = nullptr;

    quint64 qtModuleForPlugin(const QString &subDirName);
public:
    PluginsParser();
    bool scan(const QString &pluginPath, QStringList& resDependencies,
              DeployCore::QtModule qtModules);


};

#endif // QTMODULES_H
