/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "deployconfig.h"
#include "qaglobalutils.h"

#include <QFileInfo>
#include <QRegularExpression>

namespace {

/**
 * @brief selectorMatches - checks if the @a target key matches the
 * @a selector pattern (issue #311).
 *
 * The target key is the path of the target executable, so the selector
 * is checked against the file name of the target (the name of the
 * executable file) and against the full path. A selector that does not
 * contain the '*' character must be equal to the target name or to the
 * full path exactly (case-insensitive), so a plain selector selects only
 * the expected target. A selector that contains '*' is a glob pattern:
 * every '*' matches any sequence of characters (including an empty one)
 * and all the other characters are matched literally. An empty selector
 * matches every target.
 */
bool selectorMatches(const QString &target, const QString &selector) {
    if (selector.isEmpty()) {
        return true;
    }

    const QString fileName = QFileInfo(target).fileName();

    if (!selector.contains(QLatin1Char('*'))) {
        return target.compare(selector, Qt::CaseInsensitive) == 0 ||
               fileName.compare(selector, Qt::CaseInsensitive) == 0;
    }

    QString pattern;
    pattern.reserve(selector.size() + 2);
    pattern.append(QLatin1Char('^'));
    for (const QChar &ch : selector) {
        if (ch == QLatin1Char('*')) {
            pattern.append(QLatin1String(".*"));
        }
        else {
            pattern.append(QRegularExpression::escape(QString(ch)));
        }
    }
    pattern.append(QLatin1Char('$'));

    QRegularExpression matcher(pattern, QRegularExpression::CaseInsensitiveOption);
    return matcher.match(target).hasMatch() ||
           matcher.match(fileName).hasMatch();
}

} // namespace

void DeployConfig::reset() {
    *this = DeployConfig{};
}

QHash<QString, TargetInfo*>
DeployConfig::getTargetsListByFilter(const QString &filter) {
    QHash<QString, TargetInfo*> result;

    for( auto it = _targets.begin(); it != _targets.end(); ++it) {
        if (selectorMatches(it.key(), filter)) {
            result.insert(it.key(), &(*it));
        }
    }

    return result;
}

QHash<QString, const TargetInfo *>
DeployConfig::getTargetsListByFilter(const QString &filter) const {
    QHash<QString, const TargetInfo*> result;

    for( auto it = _targets.cbegin(); it != _targets.cend(); ++it) {
        if (selectorMatches(it.key(), filter)) {
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

QtMajorVersion DeployConfig::isNeededQt() const {

    auto Qt = QtMajorVersion::NoQt;
    for (const auto &i: targets()) {
        if (i.isValid()) {
            Qt = Qt | i.isDependetOfQt();
        }
    }

    return Qt;
}

QtMajorVersion DeployConfig::isNeededQt(const QString &package) const {
  const auto targetsKeys = packages().value(package, DistroModule{""}).targets();

    auto Qt = QtMajorVersion::NoQt;
    for (const auto &i: targetsKeys) {
        auto target = targets().value(i);
        if (target.isValid()) {
            Qt = Qt | target.isDependetOfQt();
        }
    }

    return Qt;
}

Platform DeployConfig::customPlatform() const {
    return _customPlatform;
}

void DeployConfig::setCustomPlatform(Platform newCustomPlatform) {
    _customPlatform = newCustomPlatform;
}

void DeployConfig::addPlatform(const LibInfo &lib) {
    platform = platform | lib.getPlatform();
}

const QHash<QString, TargetInfo> &DeployConfig::targets() const {
    return _targets;
}

const QHash<QString, DistroModule> &DeployConfig::packages() const {
    return _packages;
}


