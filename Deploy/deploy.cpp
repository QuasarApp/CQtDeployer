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
#include <stdio.h>


#include <fstream>


bool Deploy::getDeployQml() const { return deployQml; }

void Deploy::setDeployQml(bool value) { deployQml = value; }

QString Deploy::getQmlScaner() const { return externQmlScaner; }

void Deploy::setQmlScaner(const QString &value) {
    externQmlScaner = QDir::fromNativeSeparators(value);
    QuasarAppUtils::Params::verboseLog("qmlScaner = " + externQmlScaner,
                                       QuasarAppUtils::VerboseLvl::Info);
    deployQml = QFileInfo(externQmlScaner).isFile();
}

QString Deploy::getQmake() const { return qmake; }

void Deploy::setQmake(const QString &value) {
    qmake = QDir::fromNativeSeparators(value);

    QFileInfo info(qmake);
    QDir dir = info.absoluteDir();

    if (!dir.cdUp() || !dir.cd("qml")) {
        QuasarAppUtils::Params::verboseLog("get qml fail!");
        return;
    }

    qmlDir = dir.absolutePath();
    QuasarAppUtils::Params::verboseLog("qmlDir = " + qmlDir);

    dir = (info.absoluteDir());
    if (!dir.cdUp() || !dir.cd("translations")) {
        QuasarAppUtils::Params::verboseLog("get translations fail!");
        return;
    }

    translationDir = dir.absolutePath();
    QuasarAppUtils::Params::verboseLog("translations = " + translationDir);
}

bool Deploy::initDir(const QString &path) {

    if (!QFileInfo::exists(path)) {
        addToDeployed(path);
        if (!QDir().mkpath(path)) {
            return false;
        }
    }

    return true;
}

void Deploy::setTargetDir(const QString &target) {

    if (QuasarAppUtils::Params::isEndable("targetDir")) {
        targetDir = QFileInfo(QuasarAppUtils::Params::getStrArg("targetDir")).absoluteFilePath();
    } else if (target.size()) {
        targetDir = QFileInfo(target).absoluteFilePath();
    } else {
        if (targets.size())
            targetDir = QFileInfo(targets.begin().key()).absolutePath() + "/Distro";

        targetDir = QFileInfo("./Distro").absoluteFilePath();
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

bool Deploy::createRunScriptWindows(const QString &target) {

    if (distro.getBinOutDir() == distro.getLibOutDir() ) {
        return true;
    }

    QString content =
            "@echo off \n"
            "SET BASE_DIR=%~dp0 \n"
            "SET PATH = %PATH%;%BASE_DIR%" + distro.getLibOutDir() + "\n"
            "%BASE_DIR%\\" + distro.getBinOutDir() + "%0 %1 \n";

    content = content.arg(QFileInfo(target).fileName()).arg("%*");
    content = QDir::toNativeSeparators(content);

    QString fname = targetDir + QDir::separator() + QFileInfo(target).baseName()+ ".bat";

    QFile F(fname);
    if (!F.open(QIODevice::WriteOnly)) {
        return false;
    }

    F.write(content.toUtf8());
    F.flush();
    F.close();

    addToDeployed(fname);

    return F.setPermissions(QFileDevice::ExeOther | QFileDevice::WriteOther |
                            QFileDevice::ReadOther | QFileDevice::ExeUser |
                            QFileDevice::WriteUser | QFileDevice::ReadUser |
                            QFileDevice::ExeOwner | QFileDevice::WriteOwner |
                            QFileDevice::ReadOwner);
}

bool Deploy::createRunScriptLinux(const QString &target) {
    QString content =
            "#!/bin/sh\n"
            "BASE_DIR=$(dirname \"$(readlink -f \"$0\")\")\n"
            "export "
            "LD_LIBRARY_PATH=\"$BASE_DIR\"" + distro.getLibOutDir() + ":\"$BASE_DIR\":$LD_LIBRARY_PATH\n"
            "export QML_IMPORT_PATH=\"$BASE_DIR\"" + distro.getQmlOutDir() + ":QML_IMPORT_PATH\n"
            "export QML2_IMPORT_PATH=\"$BASE_DIR\"" + distro.getQmlOutDir() + ":QML2_IMPORT_PATH\n"
            "export QT_PLUGIN_PATH=\"$BASE_DIR\"" + distro.getPluginsOutDir() + ":QT_PLUGIN_PATH\n"
            "export QTDIR=\"$BASE_DIR\"\n"
            "export "
            "QT_QPA_PLATFORM_PLUGIN_PATH=\"$BASE_DIR\"" + distro.getPluginsOutDir() +
            "/platforms:QT_QPA_PLATFORM_PLUGIN_PATH\n"
            "%2"
            "\"$BASE_DIR\"" + distro.getBinOutDir() + "%1 \"$@\"";

    content = content.arg(QFileInfo(target).fileName());
    int ld_index = find("ld-linux", deployedFiles);

    if (ld_index >= 0 && QuasarAppUtils::Params::isEndable("deploySystem") &&
            !QuasarAppUtils::Params::isEndable("noLibc")) {

        content = content.arg(QString("\nexport LD_PRELOAD=\"$BASE_DIR\"" + distro.getLibOutDir() + "%0\n").
            arg(QFileInfo(deployedFiles[ld_index]).fileName()));
    } else {
        content = content.arg("");
    }

    QString fname = targetDir + QDir::separator() + QFileInfo(target).baseName()+ ".sh";

    QFile F(fname);
    if (!F.open(QIODevice::WriteOnly)) {
        return false;
    }

    F.write(content.toUtf8());
    F.flush();
    F.close();

    addToDeployed(fname);

    return F.setPermissions(QFileDevice::ExeOther | QFileDevice::WriteOther |
                            QFileDevice::ReadOther | QFileDevice::ExeUser |
                            QFileDevice::WriteUser | QFileDevice::ReadUser |
                            QFileDevice::ExeOwner | QFileDevice::WriteOwner |
                            QFileDevice::ReadOwner);
}

bool Deploy::setTargets(const QStringList &value) {

    bool isfillList = false;

    for (auto &i : value) {
        QFileInfo targetInfo(i);

        if (i.isEmpty())
            continue;

        if (targetInfo.isFile()) {

            auto sufix = targetInfo.completeSuffix();

            targets.insert(QDir::fromNativeSeparators(i), DeployUtils::isExecutable(targetInfo));
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
        targets.insert(QDir::fromNativeSeparators(file.absoluteFilePath()), DeployUtils::isExecutable(file));
    }

    return result;
}

bool Deploy::createRunScript(const QString &target) {

    QFileInfo info(target);
    auto sufix = info.completeSuffix();

    if (sufix.contains("exe", Qt::CaseSensitive)) {
        return createRunScriptWindows(target);
    }

    return createRunScriptLinux(target);
}

bool Deploy::createQConf() {

    QString content =
            "[Paths]\n"
            "Prefix= ./\n"
            "Libraries= ." + distro.getLibOutDir(distro.getBinOutDir()) + "\n"
            "Plugins= ." + distro.getPluginsOutDir(distro.getBinOutDir()) + "\n"
            "Imports= ." + distro.getQmlOutDir(distro.getBinOutDir()) + "\n"
            "Qml2Imports= ." + distro.getQmlOutDir(distro.getBinOutDir()) + "\n";


    content.replace("//", "/");
    content = QDir::fromNativeSeparators(content);

    QString fname = targetDir + distro.getBinOutDir() + "qt.conf";

    QFile F(fname);
    if (!F.open(QIODevice::WriteOnly)) {
        return false;
    }

    F.write(content.toUtf8());
    F.flush();
    F.close();

    addToDeployed(fname);

    return F.setPermissions(QFileDevice::ExeOther | QFileDevice::WriteOther |
                            QFileDevice::ReadOther | QFileDevice::ExeUser |
                            QFileDevice::WriteUser | QFileDevice::ReadUser |
                            QFileDevice::ExeOwner | QFileDevice::WriteOwner |
                            QFileDevice::ReadOwner);
}

void Deploy::initIgnoreList()
{
    if (QuasarAppUtils::Params::isEndable("ignore")) {
        auto list = QuasarAppUtils::Params::getStrArg("ignore").split(',');
        ignoreList.append(list);
    }

    if (QuasarAppUtils::Params::isEndable("noLibc")) {
        ignoreList.append("libc.so");

    }
}

void Deploy::initIgnoreEnvList()
{
    if (QuasarAppUtils::Params::isEndable("ignoreEnv")) {
        auto ignoreList = QuasarAppUtils::Params::getStrArg("ignoreEnv").split(',');

        for (auto &i : ignoreList) {
            auto path = QFileInfo(i).absoluteFilePath();

            if (path.right(1) == "/" || path.right(1) == "\\") {
                path.remove(path.size() - 1, 1);
            }

            ignoreEnvList.append(path);
        }
    }

}

void Deploy::deploy() {
    qInfo() << "target deploy started!!";

    smartMoveTargets();

    for (auto i = targets.cbegin(); i != targets.cend(); ++i) {
        extract(i.key());
    }

    copyPlugins(neededPlugins);

    if (deployQml && !extractQml()) {
        qCritical() << "qml not extacted!";
    }

    copyFiles(neadedLibs);

    if (QuasarAppUtils::Params::isEndable("deploySystem")) {
        copyFiles(systemLibs);
    }

    if (!QuasarAppUtils::Params::isEndable("noStrip") && !strip(targetDir)) {
        QuasarAppUtils::Params::verboseLog("strip failed!");
    }

    if (!QuasarAppUtils::Params::isEndable("noTranslations")) {
        if (!copyTranslations(DeployUtils::extractTranslation(neadedLibs))) {
            qWarning() << " copy TR ERROR";
        }
    }

    if (!deployMSVC()) {
        QuasarAppUtils::Params::verboseLog("deploy msvc failed");
    }

    bool targetWindows = false;

    for (auto i = targets.cbegin(); i != targets.cend(); ++i) {

        if (QFileInfo(i.key()).completeSuffix() == "exe") {
            targetWindows = true;
        }

        if (i.value() && !createRunScript(i.key())) {
            qCritical() << "run script not created!";
        }
    }

    if (targetWindows && !createQConf()) {
        QuasarAppUtils::Params::verboseLog("create qt.conf failr", QuasarAppUtils::Warning);
    }

    settings.setValue(targetDir, deployedFiles);
}

QString Deploy::getQtDir() const { return DeployUtils::qtDir; }

void Deploy::setQtDir(const QString &value) {
    DeployUtils::qtDir = QDir::fromNativeSeparators(value);
    addEnv(DeployUtils::qtDir);

    addEnv(DeployUtils::qtDir + "/lib");
    addEnv(DeployUtils::qtDir + "/bin");

}

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

int Deploy::find(const QString &str, const QStringList &list) const {
    for (int i = 0 ; i < list.size(); ++i) {
        if (list[i].contains(str))
            return i;
    }
    return -1;
}

bool Deploy::fileActionPrivate(const QString &file, const QString &target,
                               QStringList *masks, bool isMove) {

    auto info = QFileInfo(file);

    bool copy = !masks;
    if (masks) {
        for (auto mask : *masks) {
            if (info.absoluteFilePath().contains(mask)) {
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

    if (QFileInfo(file).absoluteFilePath() ==
            QFileInfo(target + QDir::separator() + name).absoluteFilePath()) {
        return true;
    }

    if (QuasarAppUtils::Params::isEndable("always-overwrite") &&
            info.exists() && !removeFile( target + QDir::separator() + name)) {
        return false;
    }

    qInfo() << ((isMove)? "move :": "copy :") << file;

    QFile sourceFile(file);

    if (!((isMove)?
          sourceFile.rename(target + QDir::separator() + name):
          sourceFile.copy(target + QDir::separator() + name))) {

        QuasarAppUtils::Params::verboseLog("Qt Operation fail " + file + " >> " + target + QDir::separator() + name +
                                           " Qt error: " + sourceFile.errorString(),
                                           QuasarAppUtils::Warning);

        std::ifstream  src(file.toStdString(),
                           std::ios::binary);
        std::ofstream  dst((target + QDir::separator() + name).toStdString(),
                           std::ios::binary);

        dst << src.rdbuf();

        if (!QFileInfo::exists(target + QDir::separator() + name)) {
            QuasarAppUtils::Params::verboseLog("std Operation fail file not copied. "
                                               "Сheck if you have access to the target dir",
                                               QuasarAppUtils::Error);
            return false;

        }
    }

    addToDeployed(target + QDir::separator() + name);
    return true;
}

void Deploy::copyFiles(const QStringList &files) {
    for (auto file : files) {
        QFileInfo target(file);
        auto targetPath = targetDir + distro.getLibOutDir();

        if (!smartCopyFile(file, targetPath)) {
            QuasarAppUtils::Params::verboseLog(file + " not copied");
        }
    }
}

bool Deploy::copyFile(const QString &file, const QString &target,
                      QStringList *masks) {

    return fileActionPrivate(file, target, masks, false);
}

bool Deploy::removeFile(const QFileInfo &file) {

    if (!QFile::remove(file.absoluteFilePath())) {
        QuasarAppUtils::Params::verboseLog("Qt Operation fail (remove file) " + file.absoluteFilePath(),
                                           QuasarAppUtils::Warning);

        if (remove(file.absoluteFilePath().toLatin1())) {
            QuasarAppUtils::Params::verboseLog("std Operation fail file not removed." + file.absoluteFilePath(),
                                               QuasarAppUtils::Error);
            return false;
        }
    }

    return true;
}

bool Deploy::removeFile(const QString &file) {
    return removeFile(QFileInfo (file));
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
            sufix.contains("exe", Qt::CaseSensitive) ||
            sufix.isEmpty() || sufix.contains("so", Qt::CaseSensitive)) {

        extractLib(file, isExtractPlugins);
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

    if ((lib.contains("Qt5Gui")) && !neededPlugins.contains("imageformats")) {
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

    if (!copyFolder(dir.absolutePath(), targetDir + distro.getPluginsOutDir() + plugin,
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
                       targetDir + distro.getPluginsOutDir() + info.baseName(),
                       QStringList() << ".so.debug" << "d.dll");
        } else {
            copyFile(info.absoluteFilePath(),
                     targetDir + distro.getPluginsOutDir());
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
        copyFile(i.absoluteFilePath(), targetDir + distro.getTrOutDir());
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
                QuasarAppUtils::Params::verboseLog(
                            item.absoluteFilePath() + " ignored by filter " + skipFilter,
                            QuasarAppUtils::VerboseLvl::Info);
                continue;
            }

            if (!copyFile(item.absoluteFilePath(), to , mask)) {
                QuasarAppUtils::Params::verboseLog(
                            "not copied file " + to + "/" + item.fileName(),
                            QuasarAppUtils::VerboseLvl::Warning);
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

void Deploy::extractLib(const QString &file, bool isExtractPlugins) {
    qInfo() << "extract lib :" << file;

    auto data = scaner.scan(file);

    for (auto &line : data) {
        bool isIgnore = false;
        for (auto ignore : ignoreList) {
            if (line.fullPath().contains(ignore)) {
                QuasarAppUtils::Params::verboseLog(line.fullPath() + " ignored by filter" + ignore);
                isIgnore = true;
                continue;
            }
        }

        if (isIgnore) {
            continue;
        }

        if (line.getPriority() != LibPriority::SystemLib && !neadedLibs.contains(line.fullPath())) {
            neadedLibs << line.fullPath();
            if (isExtractPlugins) {
                extractPlugins(line.fullPath());
            }
        } else if (QuasarAppUtils::Params::isEndable("deploySystem") &&
                    line.getPriority() == LibPriority::SystemLib &&
                    !systemLibs.contains(line.fullPath())) {
            systemLibs << line.fullPath();
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

    auto path = QFileInfo(dir).absoluteFilePath();

    for (QString & i :ignoreEnvList) {
        if (path.contains(i)) {
            return;
        }
    }

    if (path.contains(appDir)) {
        QuasarAppUtils::Params::verboseLog("is cqtdeployer dir!: " + path + " app dir : " + appDir);
        return;
    }

    if (!QFileInfo(path).isDir()) {
        QuasarAppUtils::Params::verboseLog("is not dir!! :" + path);
        return;
    }

    if (deployEnvironment.contains(path)) {
        QuasarAppUtils::Params::verboseLog ("Environment alredy added: " + path);
        return;
    }

    if (path.contains(targetDir)) {
        QuasarAppUtils::Params::verboseLog ("Skip paths becouse it is target : " + path);
        return;
    }

    deployEnvironment.push_back(QDir::fromNativeSeparators(path));
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
        auto targetPath = targetDir + (isLib(target) ? distro.getLibOutDir() : distro.getBinOutDir());

        if (!smartCopyFile(target.absoluteFilePath(), targetPath)) {
            result = false;
        }

        temp.insert(targetPath + "/" + target.fileName(), i.value());

    }

    targets = temp;

    scaner.setEnvironment(deployEnvironment);

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
    p.setProgram(externQmlScaner);
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

    if (!copyFolder(qmlDir, targetDir + distro.getQmlOutDir(),
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

    QStringList plugins;
    QStringList listItems;
    QStringList filter;
    filter << ".so.debug" << "d.dll" << ".pdb";

    if (QuasarAppUtils::Params::isEndable("qmlExtern")) {

        qInfo() << "use extern qml scaner!";

        plugins = extractImportsFromDir(info.absoluteFilePath());

    } else {
        qInfo() << "use own qml scaner!";

        QML ownQmlScaner(qmlDir);

        if (!ownQmlScaner.scan(plugins, info.absoluteFilePath())) {
            QuasarAppUtils::Params::verboseLog("qml scaner run failed!");
            return false;
        }
    }

    if (!copyFolder(qmlDir, targetDir + distro.getQmlOutDir(),
                    filter , &listItems, &plugins)) {
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

void Deploy::clear(bool force) {

    qInfo() << "clear start!";

    if (force) {
        qInfo() << "clear force! " << targetDir;

        if (QDir(targetDir).removeRecursively()) {
            return;
        }

        QuasarAppUtils::Params::verboseLog("Remove target Dir fail, try remove old deployemend files",
                                           QuasarAppUtils::Warning);
    }

    deployedFiles = settings.value(targetDir, QStringList()).toStringList();
    QMap<int, QFileInfo> sortedOldData;
    for (auto& i :deployedFiles) {
        sortedOldData.insertMulti(i.size(), QFileInfo(i));
    }

    for (auto it = sortedOldData.end(); it != sortedOldData.begin(); --it) {

        auto index = it - 1;

        if (!index.value().exists()) {
            continue;
        }

        if (index.value().isFile()) {
            if (removeFile(index.value())) {
                qInfo() << "Remove " << index.value().absoluteFilePath() << " becouse it is deployed file";
            }

        } else {
            QDir qdir(index.value().absoluteFilePath());
            if (!qdir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).count()) {
                qdir.removeRecursively();
                qInfo() << "Remove " << index.value().absoluteFilePath() << " becouse it is empty";
            }
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

bool Deploy::addToDeployed(const QString& path) {
    auto info = QFileInfo(path);
    if (info.isFile() || !info.exists()) {
        deployedFiles += info.absoluteFilePath();

        auto completeSufix = info.completeSuffix();
        if (info.isFile() && (completeSufix.isEmpty() || completeSufix.toLower() == "run"
                || completeSufix.toLower() == "sh")) {

            if (!QFile::setPermissions(path, static_cast<QFile::Permission>(0x7777))) {
                QuasarAppUtils::Params::verboseLog("permishens set fail", QuasarAppUtils::Warning);
            }
        }
    }
    return true;
}

void Deploy::initEnvirement() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    addEnv(env.value("LD_LIBRARY_PATH"));
    addEnv(env.value("PATH"));

    if (QuasarAppUtils::Params::isEndable("deploySystem")) {
        QStringList dirs;
        if (!QuasarAppUtils::Params::isEndable("noLibc"))
            dirs.append(getDirsRecursive("/lib"));
        dirs.append(getDirsRecursive("/usr/lib"));

        for (auto &&i : dirs) {
            addEnv(i);
        }
    }

    if (deployEnvironment.size() < 2) {
        qWarning() << "system environment is empty";
    }
}

QStringList Deploy::getDirsRecursive(const QString &path) {
    QDir dir(path);

    QStringList res;

    auto list = dir.entryInfoList(QDir::Dirs| QDir::NoDotAndDotDot);

    for (auto &&subDir: list) {
        res.push_back(subDir.absoluteFilePath());
        res.append(getDirsRecursive(subDir.absoluteFilePath()));
    }

    return res;
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

