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
              DeployCore::QtModule qtModules, const QString &package);


    bool initDeployPluginsList();

private:
    DependenciesScanner *_libScaner = nullptr;
    QHash<QString, QSet<QString>> _disabledPlugins;

    quint64 qtModuleForPlugin(const QString &subDirName);
    Platform platformForPlugin(const QString &name) const;

    bool copyPlugin(const QString &plugin, const QString &package);
    void copyExtraPlugins(const QString &package);
    void copyPlugins(const QStringList &list, const QString &package);
    QString getPluginNameFromFile(const QString& baseNaem) const;

    void scanPlatforms(const QString &package, QList<QString> &disabledPlugins);
    void scanPluginGroup(const QString &pluginFolder, QStringList &result, const QString &package) const;
    bool isDisavledPlugin(const QString &plugin, const QString &package) const;

    /**
     * @brief defaultForbidenPlugins - this method return list of forbiden plugins
     *  forbidenPlugin - it is a plugin that depends on several Qt modules and significantly increases the size of the distribution.
     * @return
     */
    QStringList defaultForbidenPlugins() const;
};

#endif // QTMODULES_H
