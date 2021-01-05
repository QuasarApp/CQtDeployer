//#
//# Copyright (C) 2018-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef DEPENDENCIESMAP_H
#define DEPENDENCIESMAP_H

#include "deploy_global.h"
#include "deploycore.h"
#include <QSet>

/**
 * @brief The DependenciesMap class
 * this class contains all dependencies information about target
 */
class DEPLOYSHARED_EXPORT DependencyMap
{
public:
    DependencyMap();

    DependencyMap& operator +=(const DependencyMap& other);
    DependencyMap& operator -=(const DependencyMap& other);

    DeployCore::QtModule qtModules() const;
    const QSet<QString>& neadedLibs() const;
    const QSet<QString>& systemLibs() const;
    const QSet<QString> extraData() const;
    const QSet<QString> targets() const;

    void addModule(DeployCore::QtModule module);
    void addSystemLib(const QString& lib);
    void addNeadedLib(const QString& lib);
    void addExtraData(const QString& data);

    void removeModule(DeployCore::QtModule module);
    void removeSystemLib(const QString& lib);
    void removeNeadedLib(const QString& lib);
    void removeExtraData(const QString& data);

    bool containsSysLib(const QString& lib) const;
    bool containsModule(DeployCore::QtModule module) const;
    bool containsNeadedLib(const QString& lib) const;
    bool containsExtraData(const QString& data) const;

private:
    DeployCore::QtModule _qtModules = DeployCore::NONE;

    QSet<QString> _neadedLibs;
    QSet<QString> _systemLibs;
    QSet<QString> _targets;
    QSet<QString> _extraData;

};

#endif // DEPENDENCIESMAP_H
