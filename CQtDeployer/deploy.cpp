/*
 * Copyright (C) 2018 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "deploy.h"
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <quasarapp.h>

bool Deploy::getDeployQml() const { return deployQml; }

void Deploy::setDeployQml(bool value) { deployQml = value; }

QString Deploy::getQmlScaner() const { return qmlScaner; }

void Deploy::setQmlScaner(const QString &value) {
    qmlScaner = value;
    deployQml = QFileInfo(qmlScaner).isFile();
}

QString Deploy::getQmake() const { return qmake; }

void Deploy::setQmake(const QString &value) {
    qmake = value;

    QFileInfo info(qmake);
    QDir dir = info.absoluteDir();

    if (!dir.cdUp() || !dir.cd("qml")) {
        qWarning() << "get qml fail!";
    }

    qmlDir = dir.absolutePath();
}

QString Deploy::getTarget() const { return target; }

bool Deploy::initDirs() {

    if (!QFileInfo::exists(targetDir + QDir::separator() + "lib") &&
        !QDir(targetDir).mkdir("lib")) {
        return false;
    }

    if (QuasarAppUtils::Params::isEndable("qmlDir") &&
        !QFileInfo::exists(targetDir + QDir::separator() + "qml") &&
        !QDir(targetDir).mkdir("qml")) {
        return false;
    }

    return true;
}

bool Deploy::setTarget(const QString &value) {
    target = value;

    if (target.isEmpty()) {
        return false;
    }

    targetDir = QFileInfo(target).absolutePath();

    return true;
}

bool Deploy::createRunScript() {

    QString content =
        "#!/bin/sh\n"
        "BASE_DIR=$(dirname \"$(readlink -f \"$0\")\")\n"
        "export "
        "LD_LIBRARY_PATH=\"$BASE_DIR\"/lib:\"$BASE_DIR\":$LD_LIBRARY_PATH\n"
        "export QML_IMPORT_PATH=\"$BASE_DIR\"/qml:QML_IMPORT_PATH\n"
        "export QML2_IMPORT_PATH=\"$BASE_DIR\"/qml:QML2_IMPORT_PATH\n"
        "export QT_PLUGIN_PATH=\"$BASE_DIR\"/plugins:QT_PLUGIN_PATH\n"
        "export "
        "QT_QPA_PLATFORM_PLUGIN_PATH=\"$BASE_DIR\"/plugins/"
        "platforms:QT_QPA_PLATFORM_PLUGIN_PATH\n"
        "\"$BASE_DIR\"/%1 \"$@\"";

    content = content.arg(QFileInfo(target).fileName());

    QString fname = targetDir + QDir::separator();
    if (QuasarAppUtils::Params::isEndable("runScript")) {
        fname += QuasarAppUtils::Params::getStrArg("runScript");
    } else {
        fname += "AppRun.sh";
    }

    QFile F(fname);
    if (!F.open(QIODevice::WriteOnly)) {
        return false;
    }

    F.write(content.toUtf8());
    F.flush();
    F.close();

    return F.setPermissions(QFileDevice::ExeOther | QFileDevice::WriteOther |
                            QFileDevice::ReadOther | QFileDevice::ExeUser |
                            QFileDevice::WriteUser | QFileDevice::ReadUser |
                            QFileDevice::ExeOwner | QFileDevice::WriteOwner |
                            QFileDevice::ReadOwner);
}

void Deploy::deploy() {
    qInfo() << "target deploy started!!";

    if (QuasarAppUtils::Params::isEndable("ignore")) {
        auto list = QuasarAppUtils::Params::getStrArg("ignore").split(',');
        ignoreList.append(list);
    }

    extract(target);

    if (!onlyCLibs)
        copyPlugins(neededPlugins);

    if (!onlyCLibs && deployQml && !extractQml()) {
        qCritical() << "qml not extacted!";
    }

    if (!onlyCLibs)
        copyFiles(QtLibs, targetDir + QDir::separator() + "lib");

    if (onlyCLibs || QuasarAppUtils::Params::isEndable("deploy-not-qt")) {
        copyFiles(noQTLibs, targetDir + QDir::separator() + "lib");
    }

    if (!createRunScript()) {
        qCritical() << "run script not created!";
    }
}

QString Deploy::getQtDir() const { return qtDir; }

void Deploy::setQtDir(const QString &value) { qtDir = value; }

void Deploy::setOnlyCLibs(bool value) { onlyCLibs = value; }

void Deploy::setExtraPath(const QStringList &value) {

    for (auto i : value) {
        if (QFile::exists(i)) {
            extraPath.append(i);
        } else {
            qWarning() << i << " does not exist! and skiped";
        }
    }
}

void Deploy::setExtraPlugins(const QStringList &value) {
    for (auto i : value) {
        if (QFile::exists(i)) {
            extraPlugins.append(i);
        } else {
            qWarning() << i << " does not exist! and skiped";
        }
    }
}

void Deploy::setDepchLimit(int value) { depchLimit = value; }

bool Deploy::isQtLib(const QString &lib) const {
    QFileInfo info(lib);
    return info.absolutePath().contains(qtDir);
}

void Deploy::copyFiles(const QStringList &files, const QString &target) {
    for (auto file : files) {
        if (QFileInfo(file).absolutePath() != targetDir &&
            !copyFile(file, target)) {
            qWarning() << file + " not copied";
        }
    }
}

bool Deploy::copyFile(const QString &file, const QString &target,
                      QStringList *masks) {
    auto info = QFileInfo(file);

    bool copy = !masks;
    if (masks) {
        for (auto mask : *masks) {
            if (info.absolutePath().contains(mask)) {
                copy = true;
                break;
            }
        }
    }

    if (!copy) {
        qInfo() << "skip copy :" << file;
        return false;
    }

    auto name = info.fileName();
    info.setFile(target + QDir::separator() + name);

    if (QuasarAppUtils::Params::isEndable("always-overwrite") &&
        info.exists() && !QFile::remove(target + QDir::separator() + name)) {
        return false;
    }

    qInfo() << "copy :" << target + QDir::separator() + name;

    return QFile::copy(file, target + QDir::separator() + name);
}

void Deploy::extract(const QString &file, bool isExtractPlugins) {

    qInfo() << "extract lib :" << file;

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    QString ld_path = qtDir + "/lib:" + targetDir;
    QDir dir;

    for (auto i : extraPath) {
        dir.setPath(i);
        ld_path.push_back(recursiveInvairement(0, dir));
    }

    env.insert("LD_LIBRARY_PATH", ld_path);
    env.insert("QML_IMPORT_PATH", qtDir + "/qml");
    env.insert("QML2_IMPORT_PATH", qtDir + "/qml");
    env.insert("QT_PLUGIN_PATH", qtDir + "/plugins");
    env.insert("QT_QPA_PLATFORM_PLUGIN_PATH", qtDir + "/plugins/platforms");

    QProcess P;
    P.setProcessEnvironment(env);
    P.start("ldd " + file, QProcess::ReadOnly);

    if (!P.waitForStarted())
        return;
    if (!P.waitForFinished())
        return;

    auto data = QString(P.readAll());
    QStringList libs;

    for (QString &line : data.split("\n", QString::SkipEmptyParts)) {
        line = line.simplified();
        auto innerlist = line.split(" ");

        if (innerlist.count() < 3)
            continue;
        line = innerlist[2];

        if (!line.startsWith("/")) {
            continue;
        }

        bool isIgnore = false;
        for (auto ignore : ignoreList) {
            if (line.contains(ignore)) {
                qInfo() << line << " ignored by filter" << ignore;
                isIgnore = true;
            }
        }

        if (isIgnore) {
            continue;
        }

        if (isQtLib(line) && !QtLibs.contains(line)) {
            QtLibs << line;
            extract(line, isExtractPlugins);
            if (isExtractPlugins) {
                extractPlugins(line);
            }
            continue;
        }

        if ((QuasarAppUtils::Params::isEndable("deploy-not-qt") || onlyCLibs) &&
            !noQTLibs.contains(line)) {
            noQTLibs << line;
            extract(line, isExtractPlugins);
        }
    }
}

QString Deploy::recursiveInvairement(int depch, QDir &dir) {

    if (!dir.exists() || depch >= depchLimit) {
        return ":" + dir.absolutePath();
    }

    QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QString res = "";

    for (QFileInfo &i : list) {
        dir.cd(i.fileName());
        res += ":" + recursiveInvairement(depch++, dir);
        dir.cdUp();
    }

    return res;
}

void Deploy::extractPlugins(const QString &lib) {

    qInfo() << "extrac plugin for " << lib;

    if (lib.contains("libQt5Gui") && !neededPlugins.contains("imageformats")) {
        neededPlugins << "imageformats"
                      << "iconengines"
                      << "xcbglintegrations"
                      << "platforms";
    } else if (lib.contains("libQt5Sql") &&
               !neededPlugins.contains("sqldrivers")) {
        neededPlugins << "sqldrivers";
    } else if (lib.contains("libQt5Gamepad") &&
               !neededPlugins.contains("gamepads")) {
        neededPlugins << "gamepads";
    } else if (lib.contains("libQt5PrintSupport") &&
               !neededPlugins.contains("printsupport")) {
        neededPlugins << "printsupport";
    } else if (lib.contains("libQt5Sensors") &&
               !neededPlugins.contains("sensors")) {
        neededPlugins << "sensors"
                      << "sensorgestures";
    } else if (lib.contains("libQt5Positioning") &&
               !neededPlugins.contains("geoservices")) {
        neededPlugins << "geoservices"
                      << "position"
                      << "geometryloaders";
    } else if (lib.contains("libQt5Multimedia") &&
               !neededPlugins.contains("audio")) {
        neededPlugins << "audio"
                      << "mediaservice"
                      << "playlistformats";
    }
}

bool Deploy::copyPlugin(const QString &plugin) {
    QDir dir(qtDir);
    if (!dir.cd("plugins")) {
        return false;
    }

    if (!dir.cd(plugin)) {
        return false;
    }

    QDir dirTo(targetDir);

    if (!dirTo.cd("plugins")) {
        if (!dirTo.mkdir("plugins")) {
            return false;
        }

        if (!dirTo.cd("plugins")) {
            return false;
        }
    }

    if (!dirTo.cd(plugin)) {
        if (!dirTo.mkdir(plugin)) {
            return false;
        }

        if (!dirTo.cd(plugin)) {
            return false;
        }
    }

    QStringList listItems;

    if (!copyFolder(dir, dirTo, ".so.debug", &listItems)) {
        return false;
    }

    for (auto item : listItems) {
        extract(item, false);
    }

    return true;
}

void Deploy::copyPlugins(const QStringList &list) {
    for (auto plugin : list) {
        if (!copyPlugin(plugin)) {
            qWarning() << plugin << " not copied!";
        }
    }
    QFileInfo info;

    for (auto extraPlugin : extraPlugins) {

        info.setFile(extraPlugin);
        if (info.isDir()) {
            QDir from(info.absoluteDir());
            QDir to(targetDir + QDir::separator() + "plugins");
            copyFolder(from, to, ".so.debug");
        } else {
            copyFile(info.absoluteFilePath(),
                     targetDir + QDir::separator() + "plugins");
            extract(info.absoluteFilePath());
        }
    }
}

bool Deploy::copyFolder(QDir &from, QDir &to, const QString &filter,
                        QStringList *listOfCopiedItems, QStringList *mask) {

    if (!(from.isReadable() && to.isReadable())) {
        return false;
    }

    auto list = from.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

    for (auto item : list) {
        if (QFileInfo(item).isDir()) {

            if (!from.cd(item.fileName())) {
                qWarning() << "not open "
                           << from.absolutePath() + QDir::separator() +
                                  item.fileName();
                continue;
            }

            if (!QFileInfo::exists(to.absolutePath() + QDir::separator() +
                                   item.fileName()) &&
                !to.mkdir(item.fileName())) {
                qWarning() << "not create "
                           << to.absolutePath() + QDir::separator() +
                                  item.fileName();
                continue;
            }

            if (!to.cd(item.fileName())) {
                qWarning() << "not open "
                           << to.absolutePath() + QDir::separator() +
                                  item.fileName();
                continue;
            }

            copyFolder(from, to, filter, listOfCopiedItems, mask);
            from.cdUp();
            to.cdUp();

        } else {

            if (!filter.isEmpty() && item.fileName().contains(filter)) {
                qInfo() << item << " ignored by filter " << filter;
                continue;
            }

            if (!copyFile(from.absolutePath() + QDir::separator() +
                              item.fileName(),
                          to.absolutePath(), mask)) {
                qWarning() << "not copied file "
                           << to.absolutePath() + QDir::separator() +
                                  item.fileName();
                continue;
            }

            if (listOfCopiedItems) {
                *listOfCopiedItems
                    << to.absolutePath() + QDir::separator() + item.fileName();
            }
        }
    }

    return true;
}

QStringList Deploy::findFilesInsideDir(const QString &name,
                                       const QString &dirpath) {
    QStringList files;

    QDir dir(dirpath);
    dir.setNameFilters(QStringList(name));

    QDirIterator it(dir, QDirIterator::Subdirectories);
    while (it.hasNext())
        files << it.next();

    return files;
}

QString Deploy::filterQmlPath(const QString &path) {
    if (path.contains(qmlDir)) {
        auto endIndex = path.indexOf(QDir::separator(), qmlDir.size() + 1);
        QString module =
            path.mid(qmlDir.size() + 1, endIndex - qmlDir.size() - 1);
        return qmlDir + QDir::separator() + module;
    }

    return "";
}

QStringList Deploy::extractImportsFromFiles(const QStringList &filepath) {
    QProcess p;
    p.setProgram(qmlScaner);
    p.setArguments(QStringList()
                   << "-qmlFiles" << filepath << "-importPath" << qmlDir);
    p.start();

    if (!p.waitForFinished()) {
        qWarning() << filepath << " not scaning!";
        return QStringList();
    }

    auto data = QJsonDocument::fromJson(p.readAll());

    if (!data.isArray()) {
        qWarning() << "wrong data from qml scaner! of " << filepath;
    }

    auto array = data.array();

    QStringList result;

    for (auto object : array) {

        auto module = object.toObject().value("path").toString();

        if (module.isEmpty()) {
            continue;
        }

        if (!result.contains(module)) {
            result << module;
        }
    }

    return result;
}

QStringList Deploy::extractImportsFromDir(const QString &dirpath) {
    auto files = findFilesInsideDir("*.qml", dirpath);

    QStringList result;
    result.append(extractImportsFromFiles(files));

    return result;
}

bool Deploy::extractQmlAll() {

    if (!QFileInfo::exists(qmlDir)) {
        qWarning() << "qml dir wrong!";
        return false;
    }

    QDir dir(qmlDir);

    QDir dirTo(targetDir);

    if (!dirTo.cd("qml")) {
        if (!dirTo.mkdir("qml")) {
            return false;
        }

        if (!dirTo.cd("qml")) {
            return false;
        }
    }

    QStringList listItems;

    if (!copyFolder(dir, dirTo, ".so.debug", &listItems)) {
        return false;
    }

    for (auto item : listItems) {
        extract(item, false);
    }

    return true;
}

bool Deploy::extractQmlFromSource(const QString sourceDir) {

    if (!QFileInfo::exists(qmlDir)) {
        qWarning() << "qml dir wrong!";
        return false;
    }

    QDir dir(qmlDir);

    QDir dirTo(targetDir);

    if (!dirTo.cd("qml")) {
        if (!dirTo.mkdir("qml")) {
            return false;
        }

        if (!dirTo.cd("qml")) {
            return false;
        }
    }

    QStringList plugins = extractImportsFromDir(sourceDir);

    QStringList listItems;

    if (!copyFolder(dir, dirTo, ".so.debug", &listItems, &plugins)) {
        return false;
    }

    for (auto item : listItems) {
        extract(item, false);
    }

    return true;
}

bool Deploy::extractQml() {

    if (QuasarAppUtils::Params::isEndable("qmlDir")) {
        return extractQmlFromSource(
            QuasarAppUtils::Params::getStrArg("qmlDir"));

    } else if (QuasarAppUtils::Params::isEndable("allQmlDependes")) {
        return extractQmlAll();

    } else {
        return false;
    }
}

void Deploy::clear() {

    QDir dir(targetDir);

    if (dir.cd("lib")) {
        dir.removeRecursively();
        dir.cdUp();
    }

    if (dir.cd("plugins")) {
        dir.removeRecursively();
        dir.cdUp();
    }

    if (dir.cd("qml")) {
        dir.removeRecursively();
        dir.cdUp();
    }
}

void Deploy::strip(const QString &dir) {
    QProcess P;

    P.setWorkingDirectory(dir);
    P.setArguments(QStringList() << "*");
    P.start("strip", QProcess::ReadOnly);

    if (!P.waitForStarted())
        return;
    if (!P.waitForFinished())
        return;
}

Deploy::Deploy() {}
