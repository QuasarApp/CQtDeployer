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
    return _packages.value(_targets.value(target).getPackage());
}

DistroModule DeployConfig::getDistroFromPackage(const QString &package) const {
    if (_packages.contains(package)) {
        return _packages[package];
    }

    return _packages.value("");
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

        for( auto it = disto.targets().begin(); it != disto.targets().end(); ++it) {
            result = result | _targets.value(*it).getPlatform();
        }

        return result;
    }


    for( auto it = _targets.begin(); it != _targets.end(); ++it) {
        result = result | it.value().getPlatform();
    }

    return result;


}

const QHash<QString, TargetInfo> &DeployConfig::targets() const {
    return _targets;
}

const QHash<QString, DistroModule> &DeployConfig::packages() const {
    return _packages;
}


