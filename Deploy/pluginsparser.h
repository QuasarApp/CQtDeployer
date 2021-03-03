//#
//# Copyright (C) 2018-2021 QuasarApp.
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

    /**
     * @brief defaultForbidenPlugins - this method return list of forbiden plugins
     *  forbidenPlugin - it is a plugin that depends on several Qt modules and significantly increases the size of the distribution.
     * @return
     */
    static QStringList defaultForbidenPlugins();
private:
    DependenciesScanner *_libScaner = nullptr;
    QHash<QString, QSet<QString>> _disabledPlugins;
    QHash<QString, QSet<QString>> _enabledPlugins;

    quint64 qtModuleForPluginGroup(const QString &subDirName) const;
    quint64 qtModuleForPlugin(const QString &plugin) const;

    Platform platformForPlugin(const QString &name) const;

    bool copyPlugin(const QString &plugin, const QString &package);
    void copyExtraPlugins(const QString &package);
    void copyPlugins(const QStringList &list, const QString &package);
    QString getPluginNameFromFile(const QString& baseNaem) const;

    void scanPlatforms(const QString &package, QList<QString> &disabledPlugins);
    void scanPluginGroup(const QFileInfo &pluginFolder,
                         QStringList &result,
                         const QString &package,
                         DeployCore::QtModule qtModules) const;
    bool isDisabledPlugin(const QString &plugin, const QString &package) const;
    bool isEnabledPlugin(const QString &plugin, const QString &package) const;

    void addPlugins(const QStringList &list,
                   const QString &package,
                   QHash<QString, QSet<QString>> &container);
};

#endif // QTMODULES_H
