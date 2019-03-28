/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "deploy.h"
#include "deployutils.h"
#include <QCoreApplication>
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
    QuasarAppUtils::Params::verboseLog("qmlScaner = " + qmlScaner);
    deployQml = QFileInfo(qmlScaner).isFile();
}

QString Deploy::getQmake() const { return qmake; }

void Deploy::setQmake(const QString &value) {
    qmake = QDir::fromNativeSeparators(value);

    QFileInfo info(qmake);
    QDir dir = info.absoluteDir();

    if (!dir.cdUp() || !dir.cd("qml")) {
        QuasarAppUtils::Params::verboseLog("get qml fail!");
    }

    qmlDir = dir.absolutePath();
    QuasarAppUtils::Params::verboseLog("qmlDir = " + qmlDir);

    dir = (info.absoluteDir());
    if (!dir.cdUp() || !dir.cd("translations")) {
        QuasarAppUtils::Params::verboseLog("get translations fail!");
    }

    translationDir = dir.absolutePath();
    QuasarAppUtils::Params::verboseLog("translations = " + translationDir);
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
}

bool Deploy::deployMSVC() {
    qInfo () << "try deploy msvc";

    auto msvcInstaller = DeployUtils::getVCredist(qmake);

    if (msvcInstaller.isEmpty()) {
        return false;
    }

    return copyFile(msvcInstaller, targetDir);
}

bool Deploy::setTargets(const QStringList &value) {

    bool isfillList = false;

    for (auto &i : value) {
        QFileInfo targetInfo(i);

        if (i.isEmpty())
            continue;

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
    if (dir.isEmpty() || !d.exists()) {
        DeployUtils::verboseLog(dir + " dir not exits!");
        return false;
    }
    DeployUtils::verboseLog("setBinDir check path: " + dir);
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
            "export QTDIR=\"$BASE_DIR\"\n"
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

    if (!QuasarAppUtils::Params::isEndable("noStrip") && !strip(targetDir)) {
        QuasarAppUtils::Params::verboseLog("strip failed!");
    }

    if (!onlyCLibs && !QuasarAppUtils::Params::isEndable("noTranslations")) {
        if (!copyTranslations(DeployUtils::extractTranslation(QtLibs))) {
            qWarning() << " copy TR ERROR";
        }
    }

    if (!deployMSVC()) {
        QuasarAppUtils::Params::verboseLog("deploy msvc failed");
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
        QFileInfo info(i);
        if (info.isDir()) {
            if (targets.contains(info.absoluteFilePath())) {
                QuasarAppUtils::Params::verboseLog("skip the extra lib path becouse it is target!");
                continue;
            }

            dir.setPath(info.absoluteFilePath());
            DeployUtils::extraPaths.push_back(
                        QDir::fromNativeSeparators(info.absoluteFilePath()));
            addEnv(recursiveInvairement(0, dir));
        } else {
            QuasarAppUtils::Params::verboseLog(i + " does not exist! and skiped");
        }
    }
}

void Deploy::setExtraPlugins(const QStringList &value) {
    for (auto i : value) {
        QFileInfo info(i);
        if (info.exists()) {
            extraPlugins.append(info.absoluteFilePath());
        } else {
            QuasarAppUtils::Params::verboseLog(i + " does not exist! and skiped");
        }
    }
}

void Deploy::setDepchLimit(int value) { depchLimit = value; }

bool Deploy::fileActionPrivate(const QString &file, const QString &target,
                               QStringList *masks, bool isMove) {

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
        QuasarAppUtils::Params::verboseLog(((isMove)? "skip move :": "skip copy :" + file));
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

    qInfo() << ((isMove)? "move :": "copy :") << file;

    if (!((isMove)?
          QFile::rename(file, target + QDir::separator() + name):
          QFile::copy(file, target + QDir::separator() + name))) {
        return false;
    }

    deployedFiles += target + QDir::separator() + name;

    return true;
}

void Deploy::copyFiles(const QStringList &files) {
    for (auto file : files) {
        QFileInfo target(file);
        auto targetPath = targetDir + QDir::separator() + "lib";
        if (target.completeSuffix().contains("dll", Qt::CaseInsensitive) ||
                target.completeSuffix().contains("exe", Qt::CaseInsensitive)) {

            targetPath = targetDir;
        }

        if (!smartCopyFile(file, targetPath)) {
            QuasarAppUtils::Params::verboseLog(file + " not copied");
        }
    }
}

bool Deploy::copyFile(const QString &file, const QString &target,
                      QStringList *masks) {

    return fileActionPrivate(file, target, masks, false);
}

bool Deploy::smartCopyFile(const QString &file, const QString &target, QStringList *mask) {
    if (file.contains(targetDir)) {
        if (!moveFile(file, target, mask)) {
            QuasarAppUtils::Params::verboseLog(" file not moved! try copy");

            if (!copyFile(file, target, mask)) {
                qCritical() << "not copy target to bin dir " << file;
                return false;
            };
        };
    } else {
        if (!copyFile(file, target, mask)) {
            qCritical() << "not copy target to bin dir " << file;
            return false;
        };
    }

    return true;
}

bool Deploy::moveFile(const QString &file, const QString &target, QStringList *masks) {
    return fileActionPrivate(file, target, masks, true);
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
        QuasarAppUtils::Params::verboseLog("file with sufix " + sufix + " not supported!");
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

    if (lib.contains("Qt5Gui") && !neededPlugins.contains("imageformats")) {
        neededPlugins << "imageformats"
                      << "iconengines"
                      << "xcbglintegrations"
                      << "platforms";
    } else if (lib.contains("Qt5Sql") &&
               !neededPlugins.contains("sqldrivers")) {
        neededPlugins << "sqldrivers";
    } else if (lib.contains("Qt5Gamepad") &&
               !neededPlugins.contains("gamepads")) {
        neededPlugins << "gamepads";
    } else if (lib.contains("Qt5PrintSupport") &&
               !neededPlugins.contains("printsupport")) {
        neededPlugins << "printsupport";
    } else if (lib.contains("Qt5Sensors") &&
               !neededPlugins.contains("sensors")) {
        neededPlugins << "sensors"
                      << "sensorgestures";
    } else if (lib.contains("Qt5Positioning") &&
               !neededPlugins.contains("geoservices")) {
        neededPlugins << "geoservices"
                      << "position"
                      << "geometryloaders";
    } else if (lib.contains("Qt5Multimedia") &&
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

    QStringList listItems;

    if (!copyFolder(dir.absolutePath(), targetDir + "/plugins/" + plugin,
                    QStringList() << ".so.debug" << "d.dll", &listItems)) {
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

            copyFolder(info.absoluteFilePath(),
                       targetDir + "/plugins/" + info.baseName(),
                       QStringList() << ".so.debug" << "d.dll");
        } else {
            copyFile(info.absoluteFilePath(),
                     targetDir + QDir::separator() + "plugins");
            extract(info.absoluteFilePath());
        }
    }
}

bool Deploy::copyTranslations(QStringList list) {

    QDir dir(translationDir);
    if (!dir.exists() || list.isEmpty()) {
        return false;
    }

    QStringList filters;
    for (auto &&i: list) {
        filters.push_back("*" + i + "*");
    }

    auto listItems = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

    for (auto &&i: listItems) {
        copyFile(i.absoluteFilePath(), targetDir + "/translations");
    }

    return true;
}

bool Deploy::copyFolder(const QString &from, const QString &to, const QStringList &filter,
                        QStringList *listOfCopiedItems, QStringList *mask) {

    QDir fromDir(from);

    auto list = fromDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

    for (auto &&item : list) {
        if (QFileInfo(item).isDir()) {

            copyFolder(item.absoluteFilePath(), to + "/" + item.fileName(), filter, listOfCopiedItems, mask);
        } else {

            QString skipFilter = "";
            for (auto && i: filter) {
                if (item.fileName().contains(i)) {
                    skipFilter = i;
                    break;
                }
            }

            if (!skipFilter.isEmpty()) {
                qInfo() << item.absoluteFilePath() << " ignored by filter " << skipFilter;
                continue;
            }

            if (!copyFile(item.absoluteFilePath(), to , mask)) {
                qWarning() << "not copied file " << to + "/" + item.fileName();
                continue;
            }

            if (listOfCopiedItems) {
                *listOfCopiedItems << to + "/" + item.fileName();
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

        if (innerlist.count() < 3) {
            QuasarAppUtils::Params::verboseLog(" the lib is not found: " + line);
            continue;
        }
        line = innerlist[2];

        if (!line.startsWith("/")) {
            continue;
        }

        bool isIgnore = false;
        for (auto ignore : ignoreList) {
            if (line.contains(ignore)) {
                QuasarAppUtils::Params::verboseLog(line + " ignored by filter" + ignore);
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
                QuasarAppUtils::Params::verboseLog(line + " ignored by filter" + ignore);
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

    if (dir.contains(appDir)) {
        QuasarAppUtils::Params::verboseLog("is cqtdeployer dir!: " + dir + " app dir : " + appDir);
        return;
    }

    if (!QFileInfo(dir).isDir()) {
        QuasarAppUtils::Params::verboseLog("is not dir!! :" + dir);
        return;
    }

    if (deployEnvironment.contains(dir)) {
        QuasarAppUtils::Params::verboseLog ("Environment alredy added: " + dir);
        return;
    }

    if (dir.contains(targetDir)) {
        QuasarAppUtils::Params::verboseLog ("Skip paths becouse it is target : " + dir);
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

        if (!smartCopyFile(target.absoluteFilePath(), targetPath)) {
            result = false;
        }

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

    QProcessEnvironment env;

    env.insert("LD_LIBRARY_PATH", concatEnv());
    env.insert("QML_IMPORT_PATH", DeployUtils::qtDir + "/qml");
    env.insert("QML2_IMPORT_PATH", DeployUtils::qtDir + "/qml");
    env.insert("QT_PLUGIN_PATH", DeployUtils::qtDir + "/plugins");
    env.insert("QT_QPA_PLATFORM_PLUGIN_PATH", DeployUtils::qtDir + "/plugins/platforms");

    p.setProcessEnvironment(env);
    p.setProgram(qmlScaner);
    p.setArguments(QStringList()
                   << "-rootPath" << filepath << "-importPath" << qmlDir);
    p.start();

    if (!p.waitForFinished()) {
        qWarning() << filepath << " not scaning!";
        return QStringList();
    }

    auto rawData = p.readAll();

    if (p.exitCode()) {
        qWarning() << "scaner error " << p.errorString() << "exitCode: " << p.exitCode();
    }

    QuasarAppUtils::Params::verboseLog("rawData from extractImportsFromDir: " + rawData);

    auto data = QJsonDocument::fromJson(rawData);

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

    QStringList listItems;

    if (!copyFolder(qmlDir, targetDir + "/qml",
                    QStringList() << ".so.debug" << "d.dll",
                    &listItems)) {
        return false;
    }

    for (auto item : listItems) {
        extract(item, false);
    }

    return true;
}

bool Deploy::extractQmlFromSource(const QString& sourceDir) {

    QFileInfo info(sourceDir);

    if (!info.isDir()) {
        qCritical() << "extract qml fail! qml source dir not exits or is not dir " << sourceDir;
        return false;
    }

    QuasarAppUtils::Params::verboseLog("extractQmlFromSource " + info.absoluteFilePath());

    if (!QFileInfo::exists(qmlDir)) {
        qWarning() << "qml dir wrong!";
        return false;
    }

    QStringList plugins = extractImportsFromDir(info.absoluteFilePath());
    QStringList listItems;

    for (auto &&i: plugins) {
        QuasarAppUtils::Params::verboseLog(i);
    }

    if (!copyFolder(qmlDir, targetDir + "/qml",
                    QStringList() << ".so.debug" << "d.dll",
                    &listItems, &plugins)) {
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

bool Deploy::strip(const QString &dir) {

#ifdef Q_OS_WIN
    Q_UNUSED(dir);
    return true;
#else
    QFileInfo info(dir);

    if (!info.exists()) {
        QuasarAppUtils::Params::verboseLog("dir not exits!");
        return false;
    }

    if (info.isDir()) {
        QDir d(dir);
        auto list = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

        bool res = false;
        for (auto &&i : list) {
            res = strip(i.absoluteFilePath()) || res;
        }

        return res;
    } else {

        auto sufix = info.completeSuffix();
        if (!sufix.contains("so") && !sufix.contains("dll")) {
            return false;
        }

        QProcess P;
        P.setProgram("strip");
        P.setArguments(QStringList() << info.absoluteFilePath());
        P.start();

        if (!P.waitForStarted())
            return false;
        if (!P.waitForFinished())
            return false;

        return P.exitCode() == 0;
    }
#endif
}

void Deploy::initEnvirement() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    addEnv(env.value("LD_LIBRARY_PATH"));
    addEnv(env.value("PATH"));

    if (deployEnvironment.size() < 2) {
        qWarning() << "system environment is empty";
    }
}


Deploy::Deploy() {
#ifdef Q_OS_LINUX
    appDir = QuasarAppUtils::Params::getStrArg("appPath");

    if (appDir.right(4) == "/bin") {
        appDir = appDir.left(appDir.size() - 4);
    }
#else
    appDir = QuasarAppUtils::Params::getStrArg("appPath");
#endif

    QuasarAppUtils::Params::verboseLog("appDir = " + appDir);
}

