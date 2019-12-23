#include "deployconfig.h"

void DeployConfig::reset() {
    *this = DeployConfig{};
}

QHash<QString, TargetInfo*>
DeployConfig::getTargetsListByFilter(const QString &filter) {
    QHash<QString, TargetInfo*> result;

    for( auto it = targets.begin(); it != targets.end(); ++it) {
        if (it.key().contains(filter, Qt::CaseInsensitive)) {
            result.insert(it.key(), &(*it));
        }
    }

    return result;
}

QString DeployConfig::getTargetDir(const QString &target) const {
    if (targets.contains(target))
        return targetDir + targets.value(target).getSufix();
    return targetDir;
}

void DeployConfig::setTargetDir(const QString &target) {
    targetDir = target;
}

DistroStruct DeployConfig::getDistro(const QString &target) const {
    return prefixes.value(targets.value(target).getSufix());
}

DistroStruct DeployConfig::getDistroFromPrefix(const QString &prefix) const {
    if (prefixes.contains(prefix)) {
        return prefixes[prefix];
    }

    return prefixes.value("");
}
