/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "extracter.h"
#include "deploycore.h"
#include "pluginsparser.h"
#include "cqt.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>
#include <quasarapp.h>
#include <stdio.h>


#include <fstream>

bool Extracter::deployMSVC() {
    qInfo () << "try deploy msvc";
    auto msvcInstaller = DeployCore::getVCredist(DeployCore::_config->qmake);

    if (msvcInstaller.isEmpty()) {
        return false;
    }

    return _fileManager->copyFile(msvcInstaller, DeployCore::_config->targetDir);
}

bool Extracter::copyPlugin(const QString &plugin) {

    QStringList listItems;

    if (!_fileManager->copyFolder(plugin, DeployCore::_config->targetDir + "/plugins/" + QFileInfo(plugin).fileName(),
                                 QStringList() << ".so.debug" << "d.dll", &listItems)) {
        return false;
    }

    for (auto item : listItems) {
        extract(item);
    }

    return true;
}

void Extracter::copyPlugins(const QStringList &list) {
    for (auto plugin : list) {
        if (!copyPlugin(plugin)) {
            qWarning() << plugin << " not copied!";
        }
    }
    QFileInfo info;

    for (auto extraPlugin : DeployCore::_config->extraPlugins) {

        info.setFile(extraPlugin);
        if (info.isDir()) {

            _fileManager->copyFolder(info.absoluteFilePath(),
                                    DeployCore::_config->targetDir + "/plugins/" + info.baseName(),
                                    QStringList() << ".so.debug" << "d.dll");
        } else {
            _fileManager->copyFile(info.absoluteFilePath(),
                                  DeployCore::_config->targetDir + QDir::separator() + "plugins");
            extract(info.absoluteFilePath());
        }
    }
}

bool Extracter::createRunScript(const QString &target) {

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
            "%2"
            "\"$BASE_DIR\"/bin/%1 \"$@\"";

    content = content.arg(QFileInfo(target).fileName());
    int ld_index = DeployCore::find("ld-linux", _fileManager->getDeployedFilesStringList());

    if (ld_index >= 0 && QuasarAppUtils::Params::isEndable("deploySystem") &&
            !QuasarAppUtils::Params::isEndable("noLibc")) {

        content = content.arg(QString("\nexport LD_PRELOAD=\"$BASE_DIR\"/lib/%0\n").
            arg(QFileInfo(_fileManager->getDeployedFilesStringList()[ld_index]).fileName()));
    } else {
        content = content.arg("");
    }


    QString fname = DeployCore::_config->targetDir + QDir::separator() + QFileInfo(target).baseName()+ ".sh";

    QFile F(fname);
    if (!F.open(QIODevice::WriteOnly)) {
        return false;
    }

    F.write(content.toUtf8());
    F.flush();
    F.close();

    _fileManager->addToDeployed(fname);

    return F.setPermissions(QFileDevice::ExeOther | QFileDevice::WriteOther |
                            QFileDevice::ReadOther | QFileDevice::ExeUser |
                            QFileDevice::WriteUser | QFileDevice::ReadUser |
                            QFileDevice::ExeOwner | QFileDevice::WriteOwner |
                            QFileDevice::ReadOwner);
}

bool Extracter::createQConf() {

    QString content =
            "[Paths]\n"
            "Prefix= ./\n"
            "Libraries= ./\n"
            "Plugins= ./plugins\n"
            "Imports= ./qml\n"
            "Qml2Imports= ./qml\n";


    QString fname = DeployCore::_config->targetDir + QDir::separator() + "qt.conf";

    QFile F(fname);
    if (!F.open(QIODevice::WriteOnly)) {
        return false;
    }

    F.write(content.toUtf8());
    F.flush();
    F.close();

    _fileManager->addToDeployed(fname);

    return F.setPermissions(QFileDevice::ExeOther | QFileDevice::WriteOther |
                            QFileDevice::ReadOther | QFileDevice::ExeUser |
                            QFileDevice::WriteUser | QFileDevice::ReadUser |
                            QFileDevice::ExeOwner | QFileDevice::WriteOwner |
                            QFileDevice::ReadOwner);
}



void Extracter::deploy() {
    qInfo() << "target deploy started!!";


    if (QuasarAppUtils::Params::isEndable("clear") ||
            QuasarAppUtils::Params::isEndable("force-clear")) {
        qInfo() << "clear old data";

        _fileManager->clear(DeployCore::_config->targetDir,
                            QuasarAppUtils::Params::isEndable("force-clear"));
    }

    _cqt->smartMoveTargets();

    scaner.setEnvironment(DeployCore::_config->envirement.deployEnvironment());

    for (auto i = DeployCore::_config->targets.cbegin(); i != DeployCore::_config->targets.cend(); ++i) {
        extract(i.key());
    }

    if (DeployCore::_config->deployQml && !extractQml()) {
        qCritical() << "qml not extacted!";
    }

    PluginsParser pluginsParser(&scaner);

    QStringList plugins;
    pluginsParser.scan(DeployCore::_config->qtDir + "/plugins", plugins);
    copyPlugins(plugins);


    _fileManager->copyFiles(neadedLibs, DeployCore::_config->targetDir);

    if (QuasarAppUtils::Params::isEndable("deploySystem")) {
        _fileManager->copyFiles(systemLibs, DeployCore::_config->targetDir);
    }

    if (!QuasarAppUtils::Params::isEndable("noStrip") && !_fileManager->strip(DeployCore::_config->targetDir)) {
        QuasarAppUtils::Params::verboseLog("strip failed!");
    }

    if (!QuasarAppUtils::Params::isEndable("noTranslations")) {
        if (!copyTranslations(DeployCore::extractTranslation(neadedLibs))) {
            qWarning() << " copy TR ERROR";
        }
    }

    if (!deployMSVC()) {
        QuasarAppUtils::Params::verboseLog("deploy msvc failed");
    }

    bool targetWindows = false;

    for (auto i = DeployCore::_config->targets.cbegin(); i != DeployCore::_config->targets.cend(); ++i) {

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

    _fileManager->saveDeploymendFiles(DeployCore::_config->targetDir);

    qInfo() << "deploy done!";


}

bool Extracter::copyTranslations(QStringList list) {

    QDir dir(DeployCore::_config->translationDir);
    if (!dir.exists() || list.isEmpty()) {
        return false;
    }

    QStringList filters;
    for (auto &&i: list) {
        filters.push_back("*" + i + "*");
    }

    auto listItems = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

    for (auto &&i: listItems) {
        _fileManager->copyFile(i.absoluteFilePath(), DeployCore::_config->targetDir + "/translations");
    }

    return true;
}



QFileInfoList Extracter::findFilesInsideDir(const QString &name,
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

QString Extracter::filterQmlPath(const QString &path) {
    if (path.contains(DeployCore::_config->qmlDir)) {
        auto endIndex = path.indexOf(QDir::separator(), DeployCore::_config->qmlDir.size() + 1);
        QString module =
                path.mid(DeployCore::_config->qmlDir.size() + 1, endIndex - DeployCore::_config->qmlDir.size() - 1);
        return DeployCore::_config->qmlDir + QDir::separator() + module;
    }

    return "";
}

void Extracter::extractLib(const QString &file) {
    qInfo() << "extract lib :" << file;

    auto data = scaner.scan(file);

    for (auto &line : data) {
        bool isIgnore = false;
        for (auto ignore : DeployCore::_config->ignoreList) {
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
        } else if (QuasarAppUtils::Params::isEndable("deploySystem") &&
                    line.getPriority() == LibPriority::SystemLib &&
                    !systemLibs.contains(line.fullPath())) {
            systemLibs << line.fullPath();
        }
    }
}

bool Extracter::extractQmlAll() {

    if (!QFileInfo::exists(DeployCore::_config->qmlDir)) {
        qWarning() << "qml dir wrong!";
        return false;
    }

    QStringList listItems;

    if (!_fileManager->copyFolder(DeployCore::_config->qmlDir, DeployCore::_config->targetDir + "/qml",
                    QStringList() << ".so.debug" << "d.dll",
                    &listItems)) {
        return false;
    }

    for (auto item : listItems) {
        extract(item);
    }

    return true;
}

bool Extracter::extractQmlFromSource(const QString& sourceDir) {

    QFileInfo info(sourceDir);

    if (!info.isDir()) {
        qCritical() << "extract qml fail! qml source dir not exits or is not dir " << sourceDir;
        return false;
    }

    QuasarAppUtils::Params::verboseLog("extractQmlFromSource " + info.absoluteFilePath());

    if (!QFileInfo::exists(DeployCore::_config->qmlDir)) {
        qWarning() << "qml dir wrong!";
        return false;
    }

    QStringList plugins;
    QStringList listItems;
    QStringList filter;
    filter << ".so.debug" << "d.dll" << ".pdb";

    QML ownQmlScaner(DeployCore::_config->qmlDir);

    if (!ownQmlScaner.scan(plugins, info.absoluteFilePath())) {
        QuasarAppUtils::Params::verboseLog("qml scaner run failed!");
        return false;
    }

    if (!_fileManager->copyFolder(DeployCore::_config->qmlDir, DeployCore::_config->targetDir + "/qml",
                    filter , &listItems, &plugins)) {
        return false;
    }

    for (auto item : listItems) {
        extract(item);
    }

    return true;
}

bool Extracter::extractQml() {

    if (QuasarAppUtils::Params::isEndable("qmlDir")) {
        return extractQmlFromSource(
                    QuasarAppUtils::Params::getStrArg("qmlDir"));

    } else if (QuasarAppUtils::Params::isEndable("allQmlDependes")) {
        return extractQmlAll();

    } else {
        return false;
    }
}

void Extracter::extract(const QString &file) {
    QFileInfo info(file);

    auto sufix = info.completeSuffix();

    if (sufix.contains("dll", Qt::CaseSensitive) ||
            sufix.contains("exe", Qt::CaseSensitive) ||
            sufix.isEmpty() || sufix.contains("so", Qt::CaseSensitive)) {

        extractLib(file);
    } else {
        QuasarAppUtils::Params::verboseLog("file with sufix " + sufix + " not supported!");
    }

}

Extracter::Extracter(FileManager *fileManager, CQT *cqt):
    _fileManager(fileManager),
    _cqt(cqt) {

    assert(_cqt);
    assert(_fileManager);
    assert(DeployCore::_config);
}

