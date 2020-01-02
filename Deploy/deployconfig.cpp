#include "deployconfig.h"

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
        return targetDir + _targets.value(target).getSufix();
    return targetDir;
}

void DeployConfig::setTargetDir(const QString &target) {
    targetDir = target;
}

DistroModule DeployConfig::getDistro(const QString &target) const {
    return _prefixes.value(_targets.value(target).getSufix());
}

DistroModule DeployConfig::getDistroFromPrefix(const QString &prefix) const {
    if (_prefixes.contains(prefix)) {
        return _prefixes[prefix];
    }

    return _prefixes.value("");
}

QHash<QString, TargetInfo> &DeployConfig::targetsEdit() {
    return _targets;
}

QHash<QString, DistroModule> &DeployConfig::prefixesEdit() {
    return _prefixes;
}

const QHash<QString, TargetInfo> &DeployConfig::targets() const {
    return _targets;
}

const QHash<QString, DistroModule> &DeployConfig::prefixes() const {
    return _prefixes;
}


