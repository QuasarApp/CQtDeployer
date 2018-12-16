/*
 * Copyright (C) 2018 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "deploy.h"
#include "deployutils.h"
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QRegularExpression>
#include <quasarapp.h>

bool Deploy::getDeployQml() const { return deployQml; }

void Deploy::setDeployQml(bool value) { deployQml = value; }

QString Deploy::getQmlScaner() const { return qmlScaner; }

void Deploy::setQmlScaner(const QString &value) {
    qmlScaner = QDir::fromNativeSeparators(value);
    deployQml = QFileInfo(qmlScaner).isFile();
}

QString Deploy::getQmake() const { return qmake; }

void Deploy::setQmake(const QString &value) {
    qmake = QDir::fromNativeSeparators(value);

    QFileInfo info(qmake);
    QDir dir = info.absoluteDir();

    if (!dir.cdUp() || !dir.cd("qml")) {
        qWarning() << "get qml fail!";
    }

    qmlDir = dir.absolutePath();
}

bool Deploy::initDir(const QString &path) {

    if (!QFileInfo::exists(path)) {
        deployedFiles += path;
        if (!QDir().mkpath(path)) {
            return false;
        }
    }

    return true;
}

void Deploy::setTargetDir() {
    if (QuasarAppUtils::Params::isEndable("targetDir")) {
        targetDir = QuasarAppUtils::Params::getStrArg("targetDir");

    } else {
        targetDir = QFileInfo(targets.begin().key()).absolutePath() + "/Distro";
        qInfo () << "flag targetDir not  used." << "use default target dir :" << targetDir;
    }

    addEnv(targetDir);
}

bool Deploy::setTargets(const QStringList &value) {

    bool isfillList = false;

    for (auto &i : value) {
        QFileInfo targetInfo(i);

        if (targetInfo.isFile()) {

            auto sufix = targetInfo.completeSuffix();

            targets.insert(QDir::fromNativeSeparators(i), sufix.isEmpty());
            isfillList = true;
        }
        else if (targetInfo.isDir()) {
            if (!setBinDir(i)) {
                DeployUtils::verboseLog(i + " du not contains executable binaries!");
                continue;
            }
            isfillList = true;

        } else {
            DeployUtils::verboseLog(targetInfo.absoluteFilePath() + " not exits!");
        }
    }

    if (!isfillList)
        return false;

    setTargetDir();

    return true;
}

bool Deploy::setTargetsRecursive(const QString &dir) {
    if (!setBinDir(dir, true)) {
        qWarning() << "setBinDir failed!";
        return false;
    }

    setTargetDir();

    return true;
}

bool Deploy::setBinDir(const QString &dir, bool recursive) {
    QDir d(dir);
    if (!d.exists()) {
        DeployUtils::verboseLog(dir + " dir not exits!");
        return false;
    }
    QFileInfoList list;

    if (recursive) {
        list = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    } else {
        list = d.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    }

    bool result = false;
    for (auto &file : list) {

        if (file.isDir()) {
            result |= setBinDir(file.absoluteFilePath(), recursive);
            continue;
        }

        auto sufix = file.completeSuffix();

        if (!((!recursive && sufix.isEmpty()) ||  sufix.contains("dll", Qt::CaseInsensitive) ||
              sufix.contains("so", Qt::CaseInsensitive) || sufix.contains("exe", Qt::CaseInsensitive))) {
            continue;
        }

        result = true;
        targets.insert(QDir::fromNativeSeparators(file.absoluteFilePath()), sufix.isEmpty());
    }

    return result;
}

bool Deploy::createRunScript(const QString &target) {

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
        "\"$BASE_DIR\"/bin/%1 \"$@\"";

    content = content.arg(QFileInfo(target).fileName());

    QString fname = targetDir + QDir::separator() + QFileInfo(target).baseName()+ ".sh";

    QFile F(fname);
    if (!F.open(QIODevice::WriteOnly)) {
        return false;
    }

    F.write(content.toUtf8());
    F.flush();
    F.close();

    deployedFiles += fname;

    return F.setPermissions(QFileDevice::ExeOther | QFileDevice::WriteOther |
                            QFileDevice::ReadOther | QFileDevice::ExeUser |
                            QFileDevice::WriteUser | QFileDevice::ReadUser |
                            QFileDevice::ExeOwner | QFileDevice::WriteOwner |
                            QFileDevice::ReadOwner);
}

void Deploy::createQConf() {

}

void Deploy::deploy() {
    qInfo() << "target deploy started!!";

    if (QuasarAppUtils::Params::isEndable("ignore")) {
        auto list = QuasarAppUtils::Params::getStrArg("ignore").split(',');
        ignoreList.append(list);
    }

    smartMoveTargets();

    for (auto i = targets.cbegin(); i != targets.cend(); ++i) {
        extract(i.key());

        if (i.value() && !createRunScript(i.key())) {
            qCritical() << "run script not created!";
        }
    }

    if (!onlyCLibs)
        copyPlugins(neededPlugins);

    if (!onlyCLibs && deployQml && !extractQml()) {
        qCritical() << "qml not extacted!";
    }

    if (!onlyCLibs) {
        copyFiles(QtLibs);
    }

    if (onlyCLibs || QuasarAppUtils::Params::isEndable("deploy-not-qt")) {
        copyFiles(noQTLibs);
    }

    settings.setValue(targetDir, deployedFiles);
}

QString Deploy::getQtDir() const { return DeployUtils::qtDir; }

void Deploy::setQtDir(const QString &value) {
    DeployUtils::qtDir = QDir::fromNativeSeparators(value);
    addEnv(DeployUtils::qtDir);
#ifdef Q_OS_WIN
    addEnv(DeployUtils::qtDir + "/bin");
#else
    addEnv(DeployUtils::qtDir + "/lib");
#endif
}

void Deploy::setOnlyCLibs(bool value) { onlyCLibs = value; }

void Deploy::setExtraPath(const QStringList &value) {
    QDir dir;

    for (auto i : value) {
        if (QFile::exists(i)) {
            dir.setPath(i);
            DeployUtils::extraPaths.push_back(
                        QDir::fromNativeSeparators(i));
            addEnv(recursiveInvairement(0, dir));
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

void Deploy::copyFiles(const QStringList &files) {
    for (auto file : files) {
        QFileInfo target(file);
        auto targetPath = targetDir + QDir::separator() + "lib";
        if (target.completeSuffix().contains("dll", Qt::CaseInsensitive) ||
                target.completeSuffix().contains("exe", Qt::CaseInsensitive)) {

            targetPath = targetDir;
        }

        if (target.absolutePath() != targetDir &&
            !copyFile(file, targetPath)) {
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

    if (!initDir(info.absolutePath())) {
        return false;
    }

    if (QuasarAppUtils::Params::isEndable("always-overwrite") &&
        info.exists() && !QFile::remove(target + QDir::separator() + name)) {
        return false;
    }

    qInfo() << "copy :" << target + QDir::separator() + name;

    if (!QFile::copy(file, target + QDir::separator() + name)) {
        return false;
    }

    deployedFiles += target + QDir::separator() + name;

    return true;
}

void Deploy::extract(const QString &file, bool isExtractPlugins) {
    QFileInfo info(file);

    auto sufix = info.completeSuffix();

    if (sufix.contains("dll", Qt::CaseSensitive) ||
        sufix.contains("exe", Qt::CaseSensitive)) {

        winScaner.setEnvironment(deployEnvironment);
        extractWindowsLib(file, isExtractPlugins);
    }
    else if (sufix.isEmpty() || sufix.contains("so", Qt::CaseSensitive)) {
        extractLinuxLib(file, isExtractPlugins);
    } else {
        qCritical() << "file with sufix " << sufix << " not supported!";
    }

}

QString Deploy::recursiveInvairement(int depch, QDir &dir) {

    char separator = ':';

#ifdef Q_OS_WIN
    separator = ';';
#endif

    if (!dir.exists() || depch >= depchLimit) {
        return dir.absolutePath();
    }

    QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QString res = "";

    for (QFileInfo &i : list) {
        dir.cd(i.fileName());
        QString temp = recursiveInvairement(depch + 1, dir);
        res += (res.size())? separator + temp: temp;

        dir.cdUp();
    }

    res += (res.size())? separator + dir.absolutePath(): dir.absolutePath();

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
    QDir dir(DeployUtils::qtDir);
    if (!dir.cd("plugins")) {
        return false;
    }

    if (!dir.cd(plugin)) {
        return false;
    }

    QDir dirTo(targetDir);

    if (!dirTo.cd("plugins")) {
        deployedFiles += targetDir + "/plugins";
        if (!dirTo.mkdir("plugins")) {
            return false;
        }

        if (!dirTo.cd("plugins")) {
            return false;
        }
    }

    if (!dirTo.cd(plugin)) {
        deployedFiles += dirTo.absolutePath() + "/" + plugin;

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
            QDir from(info.absoluteFilePath());
            QDir to(targetDir + QDir::separator() + "plugins" + QDir::separator() + info.baseName());
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

    if (!from.isReadable()) {
        return false;
    }

    if (!to.isReadable() && !to.mkpath(to.path())) {
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

QFileInfoList Deploy::findFilesInsideDir(const QString &name,
                                       const QString &dirpath) {
    QFileInfoList files;

    QDir dir(dirpath);

    auto list = dir.entryInfoList( QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (auto && item :list) {
        if (item.isFile()) {
            if (item.fileName().contains(name)) {
                files += item;
            }
        } else {
            files += findFilesInsideDir(name, item.absoluteFilePath());
        }
    }

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

void Deploy::extractLinuxLib(const QString &file, bool isExtractPlugins) {
    qInfo() << "extract lib :" << file;

    QProcessEnvironment env;

    env.insert("LD_LIBRARY_PATH", concatEnv());
    env.insert("QML_IMPORT_PATH", DeployUtils::qtDir + "/qml");
    env.insert("QML2_IMPORT_PATH", DeployUtils::qtDir + "/qml");
    env.insert("QT_PLUGIN_PATH", DeployUtils::qtDir + "/plugins");
    env.insert("QT_QPA_PLATFORM_PLUGIN_PATH", DeployUtils::qtDir + "/plugins/platforms");

    QProcess P;
    P.setProcessEnvironment(env);
    P.start("ldd " + file, QProcess::ReadOnly);

    if (!P.waitForStarted())
        return;
    if (!P.waitForFinished())
        return;

    auto data = QString(P.readAll());

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

        if ((DeployUtils::isQtLib(line) || DeployUtils::isExtraLib(line)) && !QtLibs.contains(line)) {
            QtLibs << line;
            extractLinuxLib(line, isExtractPlugins);
            if (isExtractPlugins) {
                extractPlugins(line);
            }
            continue;
        }

        if ((QuasarAppUtils::Params::isEndable("deploy-not-qt") || onlyCLibs) &&
            !noQTLibs.contains(line)) {
            noQTLibs << line;
            extractLinuxLib(line, isExtractPlugins);
        }
    }
}

void Deploy::extractWindowsLib(const QString &file, bool isExtractPlugins) {
    qInfo() << "extract lib :" << file;

    auto data = winScaner.scan(file, Platform::UnknownPlatform, qmake);

    for (QString &line : data) {
        line = line.simplified();

        bool isIgnore = false;
        for (auto ignore : ignoreList) {
            if (line.contains(ignore)) {
                qInfo() << line << " ignored by filter" << ignore;
                isIgnore = true;
                continue;
            }
        }

        if (isIgnore) {
            continue;
        }

        if ((DeployUtils::isQtLib(line) || DeployUtils::isExtraLib(line)) && !QtLibs.contains(line)) {
            QtLibs << line;
            extractWindowsLib(line, isExtractPlugins);
            if (isExtractPlugins) {
                extractPlugins(line);
            }
            continue;
        }

        if ((onlyCLibs || QuasarAppUtils::Params::isEndable("deploy-not-qt")) &&
            !noQTLibs.contains(line)) {
            noQTLibs << line;
            extractWindowsLib(line, isExtractPlugins);
        }
    }
}

void Deploy::addEnv(const QString &dir) {

    char separator = ':';

#ifdef Q_OS_WIN
    separator = ';';
#endif

    if (dir.contains(separator)) {
        auto list = dir.split(separator);
        for (auto i : list) {
            addEnv(i);
        }
        return;
    }

    if (!QFileInfo(dir).isDir()) {
        qWarning() << "bad Environment: " << dir;
        return;
    }

    if (deployEnvironment.contains(dir)) {
        qWarning() << "Environment alredy added: " << dir;
        return;
    }

    deployEnvironment.push_back(QDir::fromNativeSeparators(dir));
}

QString Deploy::concatEnv() const {

    if (deployEnvironment.isEmpty()) {
        return "";
    }

    QString result = deployEnvironment.first();
    for (auto i: deployEnvironment) {
        result += (":" + i);
    }

    return result;
}

bool Deploy::smartMoveTargets() {

    QMap<QString, bool> temp;
    bool result = true;
    for (auto i = targets.cbegin(); i != targets.cend(); ++i) {

        QFileInfo target(i.key());
        auto targetPath = targetDir + (isLib(target) ? "/lib" : "/bin");

        if (target.completeSuffix().contains("dll", Qt::CaseInsensitive) ||
                target.completeSuffix().contains("exe", Qt::CaseInsensitive)) {

            targetPath = targetDir;

        }

        if (!copyFile(target.absoluteFilePath(), targetPath)) {
            result = false;
            qCritical() << "not copy target to bin dir " << target.absoluteFilePath();
        };
        deployedFiles += targetPath;

        temp.insert(targetPath + "/" + target.fileName(), i.value());

    }

    targets = temp;

    return result;
}

bool Deploy::isLib(const QFileInfo &file) {
    return file.completeSuffix().contains("so", Qt::CaseInsensitive)
            || file.completeSuffix().contains("dll", Qt::CaseInsensitive);
}

QStringList Deploy::extractImportsFromDir(const QString &filepath) {
    QProcess p;
    p.setProgram(qmlScaner);
    p.setArguments(QStringList()
                   << "-rootPath" << filepath << "-importPath" << qmlDir);
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

    qInfo() << "clear start!";

    deployedFiles = settings.value(targetDir, QStringList()).toStringList();

    for (auto file : deployedFiles) {
        if (QFileInfo(file).isFile()) {
            QFile::remove(file);
        } else {
            QDir(file).removeRecursively();
        }
    }
    deployedFiles.clear();

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

void Deploy::initEnvirement() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    addEnv(env.value("LD_LIBRARY_PATH"));
    addEnv(env.value("PATH"));

    if (deployEnvironment.size() < 2) {
        qWarning() << "system environment is empty";
    }
}


Deploy::Deploy() {}

