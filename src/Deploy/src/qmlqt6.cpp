/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "qmlqt6.h"

#include <QDir>
#include <QFile>
#include <QQueue>
#include "defines.h"
#include "deploycore.h"
#include "qregularexpression.h"

QStringList QMLQt6::extractImportLine(const QString& line) const {
    QStringList result;
    QStringList list = line.split(" ", splitbehavior);

    if (list.size() <= 1) {
        return {};
    }
    
    result << (list[1].replace(".", "/"));
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
            if (!word.startsWith("import ")) continue;

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
        if (info.exists()) {
            return info.absoluteFilePath();
        }

        // Custom auto generated qml modules (created by qt_add_qml_module without
        // the NO_PLUGIN option) are built into the project build dir and are not
        // part of the Qt distribution. The module dir is resolved by scanning the
        // project dir for a qmldir that declares the module uri.
        // See the https://github.com/QuasarApp/CQtDeployer/issues/847 issue.
        if (_moduleDirs.contains(import)) {
            return _moduleDirs.value(import);
        }

        if (!_qmlProjectDir.isEmpty()) {
            auto projectInfo = QFileInfo(_qmlProjectDir + "/" + import);
            if (projectInfo.exists()) {
                return projectInfo.absoluteFilePath();
            }
        }

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

QString QMLQt6::extractModuleName(const QString &qmldirPath) {
    QFile F(qmldirPath);
    if (!F.open(QIODevice::ReadOnly)) return {};

    const auto list = QString(F.readAll()).split("\n");
    for (QString line : list) {
        line = line.simplified();
        if (line.startsWith("//") || line.startsWith("#")) continue;
        if (!line.startsWith("module")) continue;

        const auto words = line.split(" ", Qt::SkipEmptyParts);
        if (words.size() >= 2) {
            return words.at(1);
        }
    }

    return {};
}

void QMLQt6::scanProjectModules() {
    _moduleDirs.clear();

    QDir dir(_qmlProjectDir);
    if (!dir.exists() || !dir.isReadable()) {
        return;
    }

    QQueue<QDir> queue;
    queue.enqueue(dir);

    while (!queue.isEmpty()) {
        const auto d = queue.dequeue();

        const auto files = d.entryInfoList(QStringList() << "qmldir", QDir::Files);
        for (const auto &file : files) {
            auto uri = extractModuleName(file.absoluteFilePath());
            if (uri.isEmpty()) {
                continue;
            }

            // The imports are stored with "." replaced by "/" (see extractImportLine),
            // so store the module uri in the same format to allow the lookup in
            // getPathFromImport (module uri may contain dots, e.g. "My.Company.Module").
            uri.replace(".", "/");
            if (!_moduleDirs.contains(uri)) {
                _moduleDirs.insert(uri, d.absolutePath());
            }
        }

        const auto subDirs = d.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
        for (const auto &sub : subDirs) {
            queue.enqueue(QDir(sub.absoluteFilePath()));
        }
    }
}

bool QMLQt6::scan(QStringList &res, const QString& qmlProjectDir) {
    _qmlProjectDir = qmlProjectDir;

    scanProjectModules();

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
