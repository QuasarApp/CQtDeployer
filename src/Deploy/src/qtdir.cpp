/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "pathutils.h"
#include "qtdir.h"

QtMajorVersion QtDir::getQtVersion() const {
    return _qtVersion;
}

QString QtDir::toString() const {
    return QString {
        "Qt Inforamtion: \n"
        "Platform: %0\n"
        "Major version: %1\n"
        "Binaries dir: %2\n"
        "Libraries dir: %3\n"
        "Libexecs dir: %4\n"
        "Plugins dir: %5\n"
        "QMLs dir: %6\n"
        "Translations dir: %7\n"
        "Resources dir: %8\n"
    }.arg(DeployCore::platformToString(_platform),
             QString::number(DeployCore::qtVersionToString(_qtVersion)),
             bins,
             libs,
             libexecs,
             plugins,
             qmls,
             translations,
             resources);
}

QString QtDir::getLibs() const {
    return libs;
}

void QtDir::setLibs(const QString &value) {
    libs = PathUtils::fixPath(value);

    auto qtLibs = QDir(libs).entryList(QDir::Files);
    if (qtLibs.contains("libQt4Core.so")) {
        _qtVersion = QtMajorVersion::Qt4;
        _platform = Unix;
    } else if (qtLibs.contains("libQt5Core.so")) {
        _qtVersion = QtMajorVersion::Qt5;
        _platform = Unix;
    } else if (qtLibs.contains("libQt6Core.so")) {
        _qtVersion = QtMajorVersion::Qt6;
        _platform = Unix;
    } else if (qtLibs.contains("libQt4Core.dylib", Qt::CaseInsensitive)) {
        _qtVersion = QtMajorVersion::Qt4;
        _platform = Mac;
    } else if (qtLibs.contains("libQt5Core.dylib", Qt::CaseInsensitive)) {
        _qtVersion = QtMajorVersion::Qt5;
        _platform = Mac;
    } else if (qtLibs.contains("libQt6Core.dylib", Qt::CaseInsensitive)) {
        _qtVersion = QtMajorVersion::Qt6;
        _platform = Mac;
    }
}

QString QtDir::getBins() const {
    return bins;
}

void QtDir::setBins(const QString &value) {
    bins =  PathUtils::fixPath(value);

    auto qtLibs = QDir(bins).entryList(QDir::Files);
    if (qtLibs.contains("Qt4Core.dll", Qt::CaseInsensitive)) {
        _qtVersion = QtMajorVersion::Qt4;
        _platform = Win;
    } else if (qtLibs.contains("Qt5Core.dll", Qt::CaseInsensitive)) {
        _qtVersion = QtMajorVersion::Qt5;
        _platform = Win;
    } else if (qtLibs.contains("Qt6Core.dll", Qt::CaseInsensitive)) {
        _qtVersion = QtMajorVersion::Qt6;
        _platform = Win;
    }
}

QString QtDir::getLibexecs() const {
    return libexecs;
}

void QtDir::setLibexecs(const QString &value) {
    libexecs =  PathUtils::fixPath(value);
}

QString QtDir::getPlugins() const {
    return plugins;
}

void QtDir::setPlugins(const QString &value) {
    plugins =  PathUtils::fixPath(value);
}

QString QtDir::getQmls() const {
    return qmls;
}

void QtDir::setQmls(const QString &value)
{
    qmls =  PathUtils::fixPath(value);
}

QString QtDir::getTranslations() const {
    return translations;
}

void QtDir::setTranslations(const QString &value) {
    translations =  PathUtils::fixPath(value);
}

QString QtDir::getResources() const{
    return resources;
}

void QtDir::setResources(const QString &value) {
    resources =  PathUtils::fixPath(value);
}

Platform QtDir::getQtPlatform() const {
    return _platform;
}

bool QtDir::isQt(QString path) const {

    path =  PathUtils::fixPath(path);

    return
        (!libs.isEmpty() && path.contains(libs, DeployCore::getCaseSensitivity())) ||
        (!bins.isEmpty() && path.contains(bins, DeployCore::getCaseSensitivity())) ||
        (!libexecs.isEmpty() && path.contains(libexecs, DeployCore::getCaseSensitivity())) ||
        (!plugins.isEmpty() && path.contains(plugins, DeployCore::getCaseSensitivity())) ||
        (!qmls.isEmpty() && path.contains(qmls, DeployCore::getCaseSensitivity())) ||
        (!translations.isEmpty() && path.contains(translations, DeployCore::getCaseSensitivity())) ||
        (!resources.isEmpty() && path.contains(resources, DeployCore::getCaseSensitivity()));
}

QString QtDir::findMingwToolchainBin(const QString &qtBins) {

    if (qtBins.isEmpty()) {
        return {};
    }

    QDir start(qtBins);

    // The prefix root of a Qt installation is 1 to 3 levels above the bin
    // directory, depending on the layout (for example
    // D:/Qt/6.10.3/mingw_64/bin, D:/Qt/6.10.3/bin or D:/Qt/bin). Probe
    // each of them for the aqt style Tools/mingwXXXX toolchain layout.
    for (int up = 1; up <= 3; ++up) {

        QDir root = start;
        bool valid = true;

        for (int i = 0; i < up; ++i) {
            if (!root.cdUp()) {
                valid = false;
                break;
            }
        }

        if (!valid) {
            break;
        }

        QDir tools(root.filePath("Tools"));
        if (!tools.exists()) {
            continue;
        }

        QString found;
        int bestVersion = -1;

        for (const auto &entry : tools.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {

            const QString name = entry.fileName();
            if (!name.startsWith("mingw", Qt::CaseInsensitive)) {
                continue;
            }

            const QString bin = entry.absoluteFilePath() + "/bin";
            if (!QFileInfo(bin).isDir()) {
                continue;
            }

            // Only a directory that really contains the MinGW C++ runtime
            // is a valid toolchain candidate.
            if (!QFileInfo::exists(bin + "/libstdc++-6.dll")) {
                continue;
            }

            QString digits;
            for (const auto &c : name.mid(5)) {
                if (c.isDigit()) {
                    digits += c;
                }
            }

            const int version = digits.isEmpty() ? 0 : digits.toInt();
            if (version > bestVersion) {
                bestVersion = version;
                found = PathUtils::fixPath(bin);
            }
        }

        if (!found.isEmpty()) {
            return found;
        }
    }

    return {};
}
