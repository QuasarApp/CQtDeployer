/*
 * Copyright (C) 2018 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
*/

#include "deploy.h"
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <quasarapp.h>
#include <QProcess>
#include <QDirIterator>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>


bool Deploy::getDeployQml() const {
    return deployQml;
}

void Deploy::setDeployQml(bool value) {
    deployQml = value;
}

QString Deploy::getQmlScaner() const {
    return qmlScaner;
}

void Deploy::setQmlScaner(const QString &value) {
    qmlScaner = value;
    deployQml = QFileInfo(qmlScaner).isFile();
}

QString Deploy::getQmake() const {
    return qmake;
}

void Deploy::setQmake(const QString &value) {
    qmake = value;

    QFileInfo info(qmake);
    QDir dir = info.absoluteDir();

    if (!dir.cdUp() || !dir.cd("qml")) {
        qWarning() << "get qml fail!";
    }

    qmlDir = dir.absolutePath();
}

QString Deploy::getTarget() const {
    return target;
}

bool Deploy::initDirs() {

    if (!QFileInfo::exists(targetDir + QDir::separator() + "lib") &&
            !QDir(targetDir).mkdir("lib")) {
        return false;
    }

    if (QuasarAppUtils::isEndable("qmlDir") &&
            !QFileInfo::exists(targetDir  + QDir::separator() + "qml") &&
            !QDir(targetDir).mkdir("qml")){
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
        "BASEDIR=$(dirname $0)\n"
        "export LD_LIBRARY_PATH=$BASEDIR/lib:$BASEDIR:$LD_LIBRARY_PATH\n"
        "export QML_IMPORT_PATH=$BASEDIR/qml:QML_IMPORT_PATH\n"
        "export QML2_IMPORT_PATH=$BASEDIR/qml:QML2_IMPORT_PATH\n"
        "export QT_PLUGIN_PATH=$BASEDIR/plugins:QT_PLUGIN_PATH\n"
        "export QT_QPA_PLATFORM_PLUGIN_PATH=$BASEDIR/plugins/platforms:QT_QPA_PLATFORM_PLUGIN_PATH\n"
        "$BASEDIR/%1";

    content = content.arg(QFileInfo(target).fileName());

    QString fname = targetDir + QDir::separator();
    if (QuasarAppUtils::isEndable("runScript")) {
        fname += QuasarAppUtils::getStrArg("runScript");
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

    return F.setPermissions(QFileDevice::ExeUser |
                            QFileDevice::WriteUser |
                     QFileDevice::ReadUser);

}

void Deploy::deploy() {
    qInfo() << "target deploy started!!";

    if (QuasarAppUtils::isEndable("ignoreCudaLib")) {
        ignoreList << "libicudata" << "libicui" << "libicuuc";
    }

    if (QuasarAppUtils::isEndable("ignore")) {
        auto list = QuasarAppUtils::getStrArg("ignore").split(',');
        ignoreList.append(list);
    }

    extract(target);
    copyPlugins(neededPlugins);

    if (deployQml && !extractQml()) {
        qCritical() << "qml not extacted!";
    }

    copyFiles(QtLibs, targetDir + QDir::separator() + "lib");

    if (QuasarAppUtils::isEndable("deploy-not-qt")) {
        copyFiles(noQTLibs, targetDir + QDir::separator() + "lib");
    }

    if (!QuasarAppUtils::isEndable("noStrip")) {
        strip(targetDir + QDir::separator() + "lib");
    }


    if (!QuasarAppUtils::isEndable("noStrip")) {
        strip(targetDir + QDir::separator() + "plugins");
    }

    if (!createRunScript()) {
        qCritical() << "run script not created!";
    }

}

QString Deploy::getQtDir() const {
    return qtDir;
}

void Deploy::setQtDir(const QString &value) {
    qtDir = value;
}

bool Deploy::isQtLib(const QString &lib) const {
    QFileInfo info(lib);
    return info.absolutePath().contains(qtDir);
}

void Deploy::copyFiles(const QStringList &files , const QString& target) {
    for (auto file : files) {
        if (QFileInfo(file).absolutePath() != targetDir && !copyFile(file, target)) {
            qWarning() << file + " not copied";
        }
    }
}

bool Deploy::copyFile(const QString &file, const QString& target, QStringList *masks) {


    bool copy = !masks;
    if (masks) {
        for (auto mask : *masks) {
            if (file.contains(mask)) {
                copy = true;
                break;
            }
        }
    }

    if (!copy) {
        qInfo() << "skip copy :" << file ;
        return false;
    }

    auto info = QFileInfo(file);

    auto name = info.fileName();
    info.setFile(target + QDir::separator() + name);

    if (QuasarAppUtils::isEndable("always-overwrite") &&
            info.exists() && !QFile::remove(target + QDir::separator() + name)){
        return false;
    }

    qInfo() << "copy :" << target + QDir::separator() + name;

    return QFile::copy(file, target + QDir::separator() + name);

}

void Deploy::extract(const QString &file, bool isExtractPlugins) {

    qInfo() << "extract lib :" << file;

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LD_LIBRARY_PATH", qtDir + "/lib:" + targetDir);
    env.insert("QML_IMPORT_PATH", qtDir + "/qml");
    env.insert("QML2_IMPORT_PATH", qtDir + "/qml");
    env.insert("QT_PLUGIN_PATH", qtDir + "/plugins");
    env.insert("QT_QPA_PLATFORM_PLUGIN_PATH", qtDir + "/plugins/platforms");

    QProcess P;
    P.setProcessEnvironment(env);
    P.start("ldd " + file, QProcess::ReadOnly);

    if (!P.waitForStarted()) return;
    if (!P.waitForFinished()) return;

    auto data = QString(P.readAll());
    QStringList libs;

    for (QString &line : data.split("\n", QString::SkipEmptyParts)) {
        line = line.simplified();
        auto innerlist = line.split(" ");

        if (innerlist.count() < 3) continue;
        line = innerlist[2];

        if (!line.startsWith("/")) {
            continue;
        }

        bool isIgnore = false;
        for (auto ignore: ignoreList) {
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

        if (QuasarAppUtils::isEndable("deploy-not-qt") &&
                !noQTLibs.contains(line)) {
            noQTLibs << line;
            extract(line, isExtractPlugins);
        }

    }
}

void Deploy::extractPlugins(const QString &lib) {

    qInfo() << "extrac plugin for " << lib;

    if (lib.contains("libQt5Gui") && !neededPlugins.contains("imageformats")) {
        neededPlugins    << "imageformats"
                         << "iconengines"
                         << "xcbglintegrations"
                         << "platforms";
    } else if (lib.contains("libQt5Sql") && !neededPlugins.contains("sqldrivers")) {
        neededPlugins    << "sqldrivers";
    } else if (lib.contains("libQt5Gamepad") && !neededPlugins.contains("gamepads")) {
        neededPlugins    << "gamepads";
    } else if (lib.contains("libQt5PrintSupport") && !neededPlugins.contains("printsupport")) {
        neededPlugins    << "printsupport";
    } else if (lib.contains("libQt5Sensors") && !neededPlugins.contains("sensors")) {
        neededPlugins    << "sensors"
                         << "sensorgestures";
    } else if (lib.contains("libQt5Positioning") && !neededPlugins.contains("geoservices")) {
        neededPlugins    << "geoservices"
                         << "position"
                         << "geometryloaders";
    } else if (lib.contains("libQt5Multimedia") && !neededPlugins.contains("audio")) {
        neededPlugins    << "audio"
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
        if ( !copyPlugin(plugin)) {
            qWarning () << plugin << " not copied!";
        }
    }
}

bool Deploy::copyFolder( QDir &from,  QDir &to, const QString& filter,
                         QStringList* listOfCopiedItems, QStringList* mask) {

    if (!(from.isReadable() && to.isReadable())){
        return false;
    }

    auto list = from.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries);


    for (auto item : list ) {
        if (QFileInfo(item).isDir()) {

            if (!from.cd(item.fileName())) {
                qWarning() <<"not open " << from.absolutePath() + QDir::separator() + item.fileName();
                continue;
            }

            if (!QFileInfo::exists(to.absolutePath() + QDir::separator() + item.fileName()) &&
                    !to.mkdir(item.fileName())) {
                qWarning() <<"not create " << to.absolutePath() + QDir::separator() + item.fileName();
                continue;
            }

            if (!to.cd(item.fileName())) {
                qWarning() <<"not open " << to.absolutePath() + QDir::separator() + item.fileName();
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

            if (!copyFile(from.absolutePath() + QDir::separator() + item.fileName(), to.absolutePath(), mask)) {
                qWarning() <<"not copied file " << to.absolutePath() + QDir::separator() + item.fileName();
                continue;
            }

            if (listOfCopiedItems) {
                *listOfCopiedItems << to.absolutePath() + QDir::separator() + item.fileName();
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
    while (it.hasNext()) files << it.next();

    return files;
}

QString Deploy::filterQmlPath(const QString& path) {
    if (path.contains(qmlDir)) {
        auto endIndex = path.indexOf(QDir::separator(), qmlDir.size() + 1);
        QString module = path.mid(qmlDir.size() + 1, endIndex - qmlDir.size() - 1);
        return qmlDir + QDir::separator() + module;
    }

    return "";
}

QStringList Deploy::extractImportsFromFiles(const QStringList &filepath){
    QProcess p;
    p.setProgram(qmlScaner);
    p.setArguments(QStringList () << "-qmlFiles" << filepath
                   << "-importPath" << qmlDir);
    p.start();

    qInfo() << "run extract qml";

    if (!p.waitForFinished()) {
        qWarning() << filepath << " not scaning!";
        return QStringList();
    }

    auto data = QJsonDocument::fromJson(p.readAll());

    if (!data.isArray()) {
        qWarning() << "wrong data from qml scaner! of " << filepath;
    }

    auto array =  data.array();

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

    if (!QFileInfo::exists(qmlDir)){
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

    if (!QFileInfo::exists(qmlDir)){
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

    if (QuasarAppUtils::isEndable("qmlDir")) {
        return  extractQmlFromSource(QuasarAppUtils::getStrArg("qmlDir"));

    } else if (QuasarAppUtils::isEndable("allQmlDependes")){
        return  extractQmlAll();

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

void Deploy::strip(const QString& dir) {
    QProcess P;

    P.setWorkingDirectory(dir);
    P.setArguments(QStringList() << "*");
    P.start("strip", QProcess::ReadOnly);


    if (!P.waitForStarted()) return;
    if (!P.waitForFinished()) return;

}

Deploy::Deploy(){

}
