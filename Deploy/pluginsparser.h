//#
//# Copyright (C) 2018-2020 QuasarApp.
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

struct DEPLOYSHARED_EXPORT PlatformMapping
{
    const char *_pluginName;
    Platform _platform;
};

/**
 * @brief The PluginsParser class - scaner of plugins
 */
class DEPLOYSHARED_EXPORT PluginsParser
{

public:
    PluginsParser();
    bool scan(const QString &pluginPath, QStringList& resDependencies,
              DeployCore::QtModule qtModules);

    void scanPlatforms(Platform platform, QStringList& resDependencies);


private:
    DependenciesScanner *_libScaner = nullptr;

    quint64 qtModuleForPlugin(const QString &subDirName);
    Platform platformForPlugin(const QString &name);

    bool copyPlugin(const QString &plugin, const QString &package);
    void copyExtraPlugins(const QString &package);
    void copyPlugins(const QStringList &list, const QString &package);
    QString getPluginNameFromFile(const QString& baseNaem) const;
};

#endif // QTMODULES_H
