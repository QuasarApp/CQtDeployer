/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "qmlqt6.h"

#include <QDir>
#include <QFile>
#include "defines.h"
#include "deploycore.h"
#include "qregularexpression.h"

QStringList QMLQt6::extractImportLine(const QString& line) const {
    QStringList result;
    QStringList list = line.split(" ", splitbehavior);

    result << (list[1].replace(".", "/"));
    return result;

    return result;
}

QStringList QMLQt6::extractImportsFromFile(const QString &filepath) const {
    QStringList imports;
    QFile F(filepath);
    if (!F.open(QIODevice::ReadOnly)) return QStringList();

    QString content = F.readAll();
    QRegularExpression matcher;

    matcher.setPattern("\\{(.*)\\}");
    content.remove(matcher);

    matcher.setPattern("/\\*(.*)\\*/");
    content.remove(matcher);

    // Replace optional imports to restrict option, because before Qt 6.3 The Qml app needee of optional qml packas.
    content.replace("optional import", "import");


    const auto list = content.split("\n");
    for (const QString &line : list)
        for (QString &word : line.split(";", splitbehavior))
        {
            word = word.simplified();
            if (word.startsWith("//")) continue;
            if (!word.startsWith("import")) continue;

            imports += extractImportLine(word);
        }

    return imports;
}

bool QMLQt6::extractImportsFromDir(const QString &path, bool recursive) {
    QDir dir(path);

    if (!dir.isReadable()) {
        return false;
    }

    auto files = dir.entryInfoList(QStringList() << "*.qml" << "*.QML", QDir::Files);
    auto qmlmodule = dir.entryInfoList(QStringList() << "qmldir", QDir::Files);

    auto dirs = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);

    for (const auto &info: files) {
        auto imports = extractImportsFromFile(info.absoluteFilePath());
        for (const auto &import : std::as_const(imports)) {
            if (!_imports.contains(import)) {
                _imports.insert(import);
                extractImportsFromDir(getPathFromImport(import), false);
            }
        }
    }

    for (const auto& module: std::as_const(qmlmodule)) {
        QStringList imports = extractImportsFromQmlModule(module.absoluteFilePath());
        imports += extractImportsFromFile(module.absoluteFilePath());

        for (const auto &import : std::as_const(imports)) {
            if (!_imports.contains(import)) {
                _imports.insert(import);
                extractImportsFromDir(getPathFromImport(import), false);
            }
        }
    }

    // task https://github.com/QuasarApp/CQtDeployer/issues/600
    // There are no import lines for the qt models module in Qt 6.1, but this module is required for all qml applications.
    auto importQtQml = "QtQml";
    if (!_imports.contains(importQtQml)) {
        _imports.insert(importQtQml);
        extractImportsFromDir(getPathFromImport(importQtQml), false);
    }

    if (recursive) {
        for (const auto &info: dirs) {
            extractImportsFromDir(info.absoluteFilePath(), recursive);
        }
    }

    return true;
}

QString QMLQt6::getPathFromImport(const QString &import, bool checkVersions) {
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
    QRegularExpression matcher;
    matcher.setPattern("[/\\\\]");
    auto words = importData.value(index).split(matcher);
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

bool QMLQt6::deployPath(const QString &path, QStringList &res) {
    QDir dir(path);
    auto infoList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);

    for (const auto &info : std::as_const(infoList)) {
        if (DeployCore::isDebugFile(info.fileName())) {
            QuasarAppUtils::Params::log("Skip debug library " +
                                        info.absoluteFilePath());
            continue;
        }

        res.push_back(info.absoluteFilePath());
    }

    return true;
}

bool QMLQt6::scanQmlTree(const QString &qmlTree) {
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

QStringList QMLQt6::extractImportsFromQmlModule(const QString &module) const {
    QStringList imports;
    QFile F(module);
    if (!F.open(QIODevice::ReadOnly)) return QStringList();

    QString content = F.readAll();
    const auto list = content.split("\n");
    for (QString line : list) {
        line = line.simplified();
        if (line.startsWith("//") || line.startsWith("#")) continue;
        if (!line.startsWith("depends")) continue;

        imports += extractImportLine(line);
    }

    return imports;
}

QMLQt6::QMLQt6(const QString &qmlRoot): iQML(qmlRoot) {}

QMLQt6::~QMLQt6(){}

bool QMLQt6::scan(QStringList &res, const QString& _qmlProjectDir) {

    if (!scanQmlTree(_qmlRoot)) {
        return false;
    }

    if (!extractImportsFromDir(_qmlProjectDir, true)) {
        return false;
    }

    for (const auto &import : std::as_const(_imports)) {
        res.push_back(getPathFromImport(import));
    }

    return true;
}
