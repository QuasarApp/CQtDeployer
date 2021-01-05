/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "qml.h"

#include <QDir>
#include <QFile>
#include <quasarapp.h>
#include <deploycore.h>
#include "deployconfig.h"

QStringList QML::extractImportsFromFile(const QString &filepath) {
    QStringList imports;
    QFile F(filepath);
    if (!F.open(QIODevice::ReadOnly)) return QStringList();

    QString content = F.readAll();
    content.remove(QRegExp("\\{(.*)\\}"));
    content.remove(QRegExp("/\\*(.*)\\*/"));

    for (const QString &line : content.split("\n"))
        for (QString &word : line.split(";", splitbehavior))
        {
            word = word.simplified();
            if (word.startsWith("//")) continue;
            if (!word.startsWith("import")) continue;

            QStringList list = word.split(" ", splitbehavior);

            if (list.count() == 3 || (list.count() == 5  && list[3] == "as")) {
                if (list[2] == "auto") {
                    // qt6
                    imports << (list[1].replace(".", "/"));
                    continue;
                }
                // qt5
                imports << (list[2][0] + "#" + list[1].replace(".", "/"));
            } else if (list.count() == 2 || (list.count() == 4  && list[2] == "as")) {
                // qt6
                imports << (list[1].replace(".", "/"));
            }
        }

    return imports;
}

bool QML::extractImportsFromDir(const QString &path, bool recursive) {
    QDir dir(path);

    if (!dir.isReadable()) {
        return false;
    }

    auto files = dir.entryInfoList(QStringList() << "*.qml" << "*.QML", QDir::Files);
    auto dirs = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);

    for (const auto &info: files) {
        auto imports = extractImportsFromFile(info.absoluteFilePath());
        for (auto import : imports) {
            if (!_imports.contains(import)) {
                _imports.insert(import);
                extractImportsFromDir(getPathFromImport(import), recursive);
            }
        }
    }

    if (recursive) {
        for (const auto &info: dirs) {
            extractImportsFromDir(info.absoluteFilePath(), recursive);
        }
    }

    return true;
}

QString QML::getPathFromImport(const QString &import, bool checkVersions) {
    if (!import.contains("#")) {
        // qt 6
        auto info = QFileInfo(_qmlRoot + "/" + import);
        return info.absoluteFilePath();
    }

    auto importData = import.split("#");

    int index;

    if (importData.size() == 2)
        index = 1;
    else if (!importData.isEmpty()) {
        index = 0;
    } else {
        return "";
    }

    auto words = importData.value(index).split(QRegExp("[/\\\\]"));
    const bool isSecond = importData.first() == "2" && checkVersions;
    bool secondVersion = isSecond;

    QString path;
    for (auto i = words.rbegin(); i != words.rend(); ++i) {
        QString word = *i;
        if (secondVersion && secondVersions.contains(word)) {
            secondVersion = false;
            word.push_back(".2");
        }

        path.push_front(word + "/");
    }
    auto info = QFileInfo(_qmlRoot + "/" + path);

    if (isSecond && !info.exists()) {
        return getPathFromImport(import, false);
    }

    return info.absoluteFilePath();
}

bool QML::deployPath(const QString &path, QStringList &res) {
    QDir dir(path);
    auto infoList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);

    for (auto info : infoList) {
        if (DeployCore::isDebugFile(info.fileName())) {
            QuasarAppUtils::Params::log("sciped debug lib " +
                                        info.absoluteFilePath());
            continue;
        }

        res.push_back(info.absoluteFilePath());
    }

    return true;
}

bool QML::scanQmlTree(const QString &qmlTree) {
    QDir dir(qmlTree);

    if (!dir.isReadable()) {
        return false;
    }

    auto list = dir.entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot);

    for (const auto &info : list) {
        if (info.fileName().contains(".2")) {
            secondVersions.insert(info.fileName().left(info.fileName().size() - 2));
        }
        scanQmlTree(info.absoluteFilePath());

    }

    return true;
}

void QML::addImport() {

}

QML::QML(const QString &qmlRoot) {
    _qmlRoot = qmlRoot;

}

bool QML::scan(QStringList &res, const QString& _qmlProjectDir) {

    if (!scanQmlTree(_qmlRoot)) {
        return false;
    }

    if (!extractImportsFromDir(_qmlProjectDir, true)) {
        return false;
    }

    for (const auto &import : _imports) {
        res.push_back(getPathFromImport(import));
    }

    return true;
}
