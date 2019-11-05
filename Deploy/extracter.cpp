/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "extracter.h"
#include "deploycore.h"
#include "pluginsparser.h"
#include "configparser.h"
#include "metafilemanager.h"
#include "pathutils.h"
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
    auto msvcInstaller = DeployCore::getVCredist(DeployCore::_config->qtDir.bins);

    if (msvcInstaller.isEmpty()) {
        return false;
    }

    return _fileManager->copyFile(msvcInstaller, DeployCore::_config->targetDir);
}

bool Extracter::extractWebEngine() {
    auto test = static_cast<quint64>(_qtModules) & static_cast<quint64>(DeployCore::QtModule::QtWebEngineModule);
    if (test) {
        auto webEngeneBin = DeployCore::_config->qtDir.libexecs;
        if (DeployCore::_config->qtDir.qtPlatform & Platform::Unix) {
            webEngeneBin += "/QtWebEngineProcess";
        } else {
            webEngeneBin += "/QtWebEngineProcess.exe";
        }

        auto destWebEngine = DeployCore::_config->targetDir + DeployCore::_config->distroStruct.getBinOutDir();
        auto resOut = DeployCore::_config->targetDir + DeployCore::_config->distroStruct.getResOutDir();
        auto res = DeployCore::_config->qtDir.resources;

        if (!_fileManager->copyFile(webEngeneBin, destWebEngine)) {
            return false;
        }

        if (!_fileManager->copyFolder(res, resOut)) {
            return false;
        }
    }

    return true;
}

bool Extracter::copyPlugin(const QString &plugin) {

    QStringList listItems;

    auto pluginPath = DeployCore::_config->targetDir +
            DeployCore::_config->distroStruct.getPluginsOutDir() +
            QFileInfo(plugin).fileName();

    if (!_fileManager->copyFolder(plugin, pluginPath,
                    QStringList() << ".so.debug" << "d.dll", &listItems)) {
        return false;
    }

    if (QuasarAppUtils::Params::isEndable("dPlugin")) {
        return true;
    }

    for (auto item : listItems) {
        extract(item);
    }

    return true;
}

void Extracter::copyExtraPlugins() {
    QFileInfo info;

    for (auto extraPlugin : DeployCore::_config->extraPlugins) {

        if (!PathUtils::isPath(extraPlugin)) {
            extraPlugin = DeployCore::_config->qtDir.plugins + "/" + extraPlugin;
        }

        info.setFile(extraPlugin);
        if (info.isDir() && DeployCore::_config->qtDir.isQt(info.absoluteFilePath())) {
            copyPlugin(info.absoluteFilePath());

        } else if (info.exists()) {
            _fileManager->copyFile(info.absoluteFilePath(),
                                  DeployCore::_config->targetDir + DeployCore::_config->distroStruct.getPluginsOutDir());

            if (QuasarAppUtils::Params::isEndable("dPlugin")) {
                continue;
            }

            extract(info.absoluteFilePath());
        }
    }
}

void Extracter::copyPlugins(const QStringList &list) {
    for (auto plugin : list) {        
        if (!copyPlugin(plugin)) {
            qWarning() << plugin << " not copied!";
        }
    }
    copyExtraPlugins();
}

void Extracter::extractAllTargets() {
    for (auto i = DeployCore::_config->targets.cbegin(); i != DeployCore::_config->targets.cend(); ++i) {
        extract(*i);
    }
}

void Extracter::initQtModules() {
    for (auto i: neadedLibs) {
        DeployCore::addQtModule(_qtModules, i);
    }
}

void Extracter::clear() {
    if (QuasarAppUtils::Params::isEndable("clear") ||
            QuasarAppUtils::Params::isEndable("force-clear")) {
        qInfo() << "clear old data";

        _fileManager->clear(DeployCore::_config->targetDir,
                            QuasarAppUtils::Params::isEndable("force-clear"));
    }
}

void Extracter::extractPlugins()
{
    PluginsParser pluginsParser;

    QStringList plugins;
    pluginsParser.scan(DeployCore::_config->qtDir.plugins, plugins, _qtModules);
    copyPlugins(plugins);
}

void Extracter::copyFiles()
{
    _fileManager->copyLibs(neadedLibs);

    if (QuasarAppUtils::Params::isEndable("deploySystem")) {
        _fileManager->copyLibs(systemLibs);
    }

    if (!QuasarAppUtils::Params::isEndable("noStrip") && !_fileManager->strip(DeployCore::_config->targetDir)) {
        QuasarAppUtils::Params::verboseLog("strip failed!");
    }
}

void Extracter::copyTr()
{
    if (!QuasarAppUtils::Params::isEndable("noTranslations")) {
        if (!copyTranslations(DeployCore::extractTranslation(neadedLibs))) {
            qWarning() << " copy TR ERROR";
        }
    }
}

void Extracter::deploy() {
    qInfo() << "target deploy started!!";

    clear();
    _cqt->smartMoveTargets();
    scaner.setEnvironment(DeployCore::_config->envirement.deployEnvironment());
    extractAllTargets();

    if (DeployCore::_config->deployQml && !extractQml()) {
        qCritical() << "qml not extacted!";
    }

    initQtModules();

    extractPlugins();

    copyFiles();

    copyTr();

    if (!extractWebEngine()) {
        QuasarAppUtils::Params::verboseLog("deploy webEngine failed", QuasarAppUtils::Error);
    }

    if (!deployMSVC()) {
        QuasarAppUtils::Params::verboseLog("deploy msvc failed");
    }

    _metaFileManager->createRunMetaFiles();

    qInfo() << "deploy done!";

}

bool Extracter::copyTranslations(QStringList list) {

    QDir dir(DeployCore::_config->qtDir.translations);
    if (!dir.exists() || list.isEmpty()) {
        return false;
    }

    QStringList filters;
    for (auto &&i: list) {
        filters.push_back("*" + i + "*");
    }

    auto listItems = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

    for (auto &&i: listItems) {
        _fileManager->copyFile(i.absoluteFilePath(), DeployCore::_config->targetDir + DeployCore::_config->distroStruct.getTrOutDir());
    }

    auto webEngine = static_cast<quint64>(_qtModules) & static_cast<quint64>(DeployCore::QtModule::QtWebEngineModule);

    if (webEngine) {
        auto trOut = DeployCore::_config->targetDir + DeployCore::_config->distroStruct.getTrOutDir();
        auto tr = DeployCore::_config->qtDir.translations + "/qtwebengine_locales";
        _fileManager->copyFolder(tr, trOut + "/qtwebengine_locales");
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

void Extracter::extractLib(const QString &file) {
    qInfo() << "extract lib :" << file;

    auto data = scaner.scan(file);

    for (auto &line : data) {
        if (DeployCore::_config->ignoreList.isIgnore(line)) {
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

    if (!QFileInfo::exists(DeployCore::_config->qtDir.qmls)) {
        qWarning() << "qml dir wrong!";
        return false;
    }

    QStringList listItems;

    if (!_fileManager->copyFolder(DeployCore::_config->qtDir.qmls,
                                 DeployCore::_config->targetDir + DeployCore::_config->distroStruct.getQmlOutDir(),
                    QStringList() << ".so.debug" << "d.dll" << ".pdb",
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

    if (!QFileInfo::exists(DeployCore::_config->qtDir.qmls)) {
        qWarning() << "qml dir wrong!";
        return false;
    }

    QStringList plugins;
    QStringList listItems;
    QStringList filter;
    filter << ".so.debug" << "d.dll" << ".pdb";

    QML ownQmlScaner(DeployCore::_config->qtDir.qmls);

    if (!ownQmlScaner.scan(plugins, info.absoluteFilePath())) {
        QuasarAppUtils::Params::verboseLog("qml scaner run failed!");
        return false;
    }

    if (!_fileManager->copyFolder(DeployCore::_config->qtDir.qmls,
                                 DeployCore::_config->targetDir + DeployCore::_config->distroStruct.getQmlOutDir(),
                    filter , &listItems, &plugins)) {
        return false;
    }

    if (QuasarAppUtils::Params::isEndable("dQml")) {
        return true;
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

    if (sufix.compare("dll", Qt::CaseSensitive) == 0 ||
            sufix.compare("exe", Qt::CaseSensitive) == 0 ||
            sufix.isEmpty() || sufix.contains("so", Qt::CaseSensitive)) {

        extractLib(file);
    } else {
        QuasarAppUtils::Params::verboseLog("file with sufix " + sufix + " not supported!");
    }

}

Extracter::Extracter(FileManager *fileManager, ConfigParser *cqt):
    _fileManager(fileManager),
    _cqt(cqt) {

    _qtModules = DeployCore::QtModule::NONE;

    assert(_cqt);
    assert(_fileManager);
    assert(DeployCore::_config);

    _metaFileManager = new MetaFileManager(_fileManager);
}

