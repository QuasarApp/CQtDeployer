/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "deployconfig.h"
#include "quasarapp.h"

void DeployConfig::reset() {
    *this = DeployConfig{};
}

QHash<QString, TargetInfo*>
DeployConfig::getTargetsListByFilter(const QString &filter) {
    QHash<QString, TargetInfo*> result;

    for( auto it = _targets.begin(); it != _targets.end(); ++it) {
        if (it.key().contains(filter, Qt::CaseInsensitive)) {
            result.insert(it.key(), &(*it));
        }
    }

    return result;
}

QString DeployConfig::getTargetDir(const QString &target) const {
    if (_targets.contains(target))
        return targetDir + "/" + _targets.value(target).getPackage();
    return targetDir;
}

QString DeployConfig::getPackageTargetDir(const QString &package) const {
    if (!_packages.contains(package)) {
#ifdef QT_DEBUG
        abort();
#endif
        return "";
    }

    return targetDir + "/" + package;

}

void DeployConfig::setTargetDir(const QString &target) {
    targetDir = target;

    if (targetDir.right(1) != "/" ) {
        ignoreList.addRule(targetDir + "/");
    }
    else {
        ignoreList.addRule(targetDir);
    }

}

DistroModule DeployConfig::getDistro(const QString &target) const {
    auto key = _targets.value(target).getPackage();
    return _packages.value(key, DistroModule{key});
}

DistroModule DeployConfig::getDistroFromPackage(const QString &package) const {
    return _packages.value(package, DistroModule{package});
}

QHash<QString, TargetInfo> &DeployConfig::targetsEdit() {
    return _targets;
}

QHash<QString, DistroModule> &DeployConfig::packagesEdit() {
    return _packages;
}

Platform DeployConfig::getPlatform(const QString& package) const {
    Platform result = Platform::UnknownPlatform;

    if (_packages.contains(package)) {
        auto disto = getDistroFromPackage(package);

        for( auto it = disto.targets().cbegin(); it != disto.targets().cend(); ++it) {
            result = result | _targets.value(*it).getPlatform();
        }
    }

    return result;
}

Platform DeployConfig::getPlatformOfAll() const {
    Platform result = Platform::UnknownPlatform;

    for( auto it = _targets.cbegin(); it != _targets.cend(); ++it) {
        result = result | it.value().getPlatform();
    }

    return result;
}

QString DeployConfig::getDefaultPackage() const {
    return defaultPackage;
}

void DeployConfig::setDefaultPackage(const QString &value) {
    defaultPackage = value;
}

void DeployConfig::registerRunScript(const QString &targetName,
                                     const QString &scriptPath) {
    _runScripts.insert(targetName, scriptPath);
}

QString DeployConfig::getRunScript(const QString &targetName) const {
    return _runScripts.value(targetName, "");
}

const QHash<QString, TargetInfo> &DeployConfig::targets() const {
    return _targets;
}

const QHash<QString, DistroModule> &DeployConfig::packages() const {
    return _packages;
}


