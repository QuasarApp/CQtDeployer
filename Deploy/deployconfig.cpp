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

QString DeployConfig::targetPath(const QString &target) {
    if (targets.contains(target))
        return targetDir + targets.value(target).getSufix();
    return targetDir;
}

bool QtDir::isQt(const QString& path) const {

    return
    (!libs.isEmpty() && path.contains(libs)) ||
    (!bins.isEmpty() && path.contains(bins)) ||
    (!libexecs.isEmpty() && path.contains(libexecs)) ||
    (!plugins.isEmpty() && path.contains(plugins)) ||
    (!qmls.isEmpty() && path.contains(qmls)) ||
    (!translations.isEmpty() && path.contains(translations)) ||
    (!resources.isEmpty() && path.contains(resources));
}

bool Extra::contains(const QString &path) const {
    QFileInfo info(path);
    if (extraPaths.contains(info.absolutePath())) {
        return true;
    }

    for (auto i: extraPathsMasks) {
        if (info.absoluteFilePath().contains(i)) {
            return true;
        }
    }

    for (auto i: extraNamesMasks) {
        if (info.fileName().contains(i)) {
            return true;
        }
    }

    return false;
}
