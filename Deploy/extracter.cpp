/*
 * Copyright (C) 2018-2020 QuasarApp.
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
#include <cstdio>

#include <cassert>

#include <fstream>

bool Extracter::deployMSVC() {
    QuasarAppUtils::Params::log("try deploy msvc",
                                       QuasarAppUtils::Info);
    auto msvcInstaller = DeployCore::getVCredist(DeployCore::_config->qtDir.getBins());

    if (msvcInstaller.isEmpty()) {
        return false;
    }

    return _fileManager->copyFile(msvcInstaller, DeployCore::_config->getTargetDir());
}

bool Extracter::isWebEngine(const QString &package) const {
    auto qtModules = _packageDependencyes.value(package).qtModules();

    return static_cast<quint64>(qtModules) & static_cast<quint64>(DeployCore::QtModule::QtWebEngineModule);
}

bool Extracter::extractWebEngine() {

    auto cnf = DeployCore::_config;

    for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {

        const auto &package = i.key();
        if (isWebEngine(package)) {
            auto webEngeneBin = cnf->qtDir.getLibexecs();
            if (cnf->qtDir.getQtPlatform() & Platform::Unix) {
                webEngeneBin += "/QtWebEngineProcess";
            } else {
                webEngeneBin += "/QtWebEngineProcess.exe";
            }

            auto destWebEngine = cnf->getTargetDir() + "/" + package + cnf->packages()[package].getBinOutDir();
            auto resOut = cnf->getTargetDir() + "/" + package + cnf->packages()[package].getResOutDir();
            auto res = cnf->qtDir.getResources();

            if (!_fileManager->copyFile(webEngeneBin, destWebEngine)) {
                return false;
            }

            if (!_fileManager->copyFolder(res, resOut)) {
                return false;
            }
        }
    }

    return true;
}

bool Extracter::copyPlugin(const QString &plugin, const QString& package) {

    QStringList listItems;

    auto cnf = DeployCore::_config;
    auto targetPath = cnf->getTargetDir() + "/" + package;
    auto distro = cnf->getDistroFromPackage(package);


    auto pluginPath = targetPath + distro.getPluginsOutDir() +
            QFileInfo(plugin).fileName();

    if (!_fileManager->copyFolder(plugin, pluginPath,
                    QStringList() << ".so.debug" << "d.dll" << ".pdb", &listItems)) {
        return false;
    }

    for (const auto &item : listItems) {
        extractPluginLib(item, package);
    }

    return true;
}

void Extracter::copyExtraPlugins(const QString& package) {
    QFileInfo info;

    auto cnf = DeployCore::_config;
    auto targetPath = cnf->getTargetDir() + "/" + package;
    auto distro = cnf->getDistroFromPackage(package);

    for (auto extraPlugin : cnf->extraPlugins) {

        if (!PathUtils::isPath(extraPlugin)) {
            extraPlugin = cnf->qtDir.getPlugins() + "/" + extraPlugin;
        }

        info.setFile(extraPlugin);
        if (info.isDir() && cnf->qtDir.isQt(info.absoluteFilePath())) {
            copyPlugin(info.absoluteFilePath(), package);

        } else if (info.exists()) {
            _fileManager->copyFile(info.absoluteFilePath(),
                                  targetPath + distro.getPluginsOutDir());

            extractPluginLib(info.absoluteFilePath(), package);
        }
    }
}

void Extracter::copyPlugins(const QStringList &list, const QString& package) {
    for (const auto &plugin : list) {
        if (!copyPlugin(plugin, package)) {
            QuasarAppUtils::Params::log("not copied!",
                                               QuasarAppUtils::Warning);
        }
    }
    copyExtraPlugins(package);
}

void Extracter::extractAllTargets() {
    auto cfg = DeployCore::_config;
    for (auto i = cfg->packages().cbegin(); i != cfg->packages().cend(); ++i) {
        _packageDependencyes[i.key()] = {};

        for (const auto &target : i.value().targets()) {
            extract(target, &_packageDependencyes[i.key()]);
        }
    }
}

void Extracter::clear() {
    if (QuasarAppUtils::Params::isEndable("clear") ||
            QuasarAppUtils::Params::isEndable("force-clear")) {
        QuasarAppUtils::Params::log("clear old data",
                                           QuasarAppUtils::Info);
        _fileManager->clear(DeployCore::_config->getTargetDir(),
                            QuasarAppUtils::Params::isEndable("force-clear"));
    }
}

void Extracter::extractPlugins() {
    auto cnf = DeployCore::_config;
    PluginsParser pluginsParser;

    for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {
        auto distro = cnf->getDistroFromPackage(i.key());

        QStringList plugins;
        pluginsParser.scan(cnf->qtDir.getPlugins(), plugins, _packageDependencyes[i.key()].qtModules());
        copyPlugins(plugins, i.key());
    }
}

void Extracter::copyLibs(const QSet<QString> &files, const QString& package) {
    auto cnf = DeployCore::_config;
    auto targetPath = cnf->getTargetDir() + "/" + package;
    auto distro = cnf->getDistroFromPackage(package);

    for (const auto &file : files) {
        if (!_fileManager->smartCopyFile(file, targetPath + distro.getLibOutDir())) {
            QuasarAppUtils::Params::log(file + " not copied");
        }
    }
}

void Extracter::copyFiles() {
    auto cnf = DeployCore::_config;

    for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {

        copyLibs(_packageDependencyes[i.key()].neadedLibs(), i.key());

        if (QuasarAppUtils::Params::isEndable("deploySystem")) {
            copyLibs(_packageDependencyes[i.key()].systemLibs(), i.key());
        }

        if (!QuasarAppUtils::Params::isEndable("noStrip") && !_fileManager->strip(cnf->getTargetDir())) {
            QuasarAppUtils::Params::log("strip failed!");
        }
    }
}

void Extracter::copyTr() {

    if (!QuasarAppUtils::Params::isEndable("noTranslations")) {

        auto cnf = DeployCore::_config;

        for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {
            if (!copyTranslations(DeployCore::extractTranslation(_packageDependencyes[i.key()].neadedLibs()),
                                  i.key())) {
                QuasarAppUtils::Params::log("Failed to copy standard Qt translations",
                                                   QuasarAppUtils::Warning);
            }
        }

    }
}

void Extracter::deploy() {
    QuasarAppUtils::Params::log("target deploy started!!",
                                       QuasarAppUtils::Info);
    clear();
    _cqt->smartMoveTargets();
    _scaner->setEnvironment(DeployCore::_config->envirement.environmentList());
    extractAllTargets();

    if (DeployCore::_config->deployQml && !extractQml()) {
        QuasarAppUtils::Params::log("qml not extacted!",
                                           QuasarAppUtils::Error);
    }

    extractPlugins();

    copyFiles();

    copyTr();

    if (!extractWebEngine()) {
        QuasarAppUtils::Params::log("deploy webEngine failed", QuasarAppUtils::Error);
    }

    if (!deployMSVC()) {
        QuasarAppUtils::Params::log("deploy msvc failed");
    }

    _metaFileManager->createRunMetaFiles();
    QuasarAppUtils::Params::log("deploy done!",
                                       QuasarAppUtils::Info);

}

bool Extracter::copyTranslations(const QStringList &list, const QString& package) {

    auto cnf = DeployCore::_config;

    QDir dir(cnf->qtDir.getTranslations());
    if (!dir.exists() || list.isEmpty()) {
        return false;
    }

    QStringList filters;
    for (const auto &i: list) {
        filters.push_back("*" + i + "*");
    }

    auto listItems = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

    auto targetPath = cnf->getTargetDir() + "/" + package;
    auto distro = cnf->getDistroFromPackage(package);

    for (const auto &i: listItems) {
        _fileManager->copyFile(i.absoluteFilePath(), targetPath + distro.getTrOutDir());
    }

    if (isWebEngine(package)) {
        auto trOut = targetPath + distro.getTrOutDir();
        auto tr = cnf->qtDir.getTranslations() + "/qtwebengine_locales";
        _fileManager->copyFolder(tr, trOut + "/qtwebengine_locales");
    }

    return true;
}



QFileInfoList Extracter::findFilesInsideDir(const QString &name,
                                         const QString &dirpath) {
    QFileInfoList files;

    QDir dir(dirpath);

    auto list = dir.entryInfoList( QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const auto & item :list) {
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

void Extracter::extractLib(const QString &file,
                           DependencyMap* depMap,
                           const QString& mask) {

    assert(depMap);
    QuasarAppUtils::Params::log("extract lib :" + file,
                                       QuasarAppUtils::Info);

    auto data = _scaner->scan(file);

    for (const auto &line : data) {

        if (mask.size() && !line.getName().contains(mask, ONLY_WIN_CASE_INSENSIATIVE)) {
            continue;
        }

        if (DeployCore::_config->ignoreList.isIgnore(line)) {
            continue;
        }

        if (line.getPriority() < LibPriority::SystemLib && !depMap->containsNeadedLib(line.fullPath())) {
            depMap->addNeadedLib(line.fullPath());

        } else if (QuasarAppUtils::Params::isEndable("deploySystem") &&
                    line.getPriority() >= LibPriority::SystemLib &&
                    !depMap->containsSysLib(line.fullPath())) {

            depMap->addSystemLib(line.fullPath());
        }
    }
}

void Extracter::extractPluginLib(const QString& item, const QString& package) {
    if (QuasarAppUtils::Params::isEndable("extractPlugins")) {
        extract(item, &_packageDependencyes[package]);
    } else {
        extract(item, &_packageDependencyes[package], "Qt");
    }
}

bool Extracter::extractQmlAll() {
    auto cnf = DeployCore::_config;

    if (!QFileInfo::exists(cnf->qtDir.getQmls())) {
        QuasarAppUtils::Params::log("qml dir wrong!",
                                           QuasarAppUtils::Warning);
        return false;
    }

    for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {
        auto targetPath = cnf->getTargetDir() + "/" + i.key();
        auto distro = cnf->getDistroFromPackage(i.key());

        QStringList listItems;

        if (!_fileManager->copyFolder(cnf->qtDir.getQmls(), targetPath + distro.getQmlOutDir(),
                        QStringList() << ".so.debug" << "d.dll" << ".pdb",
                        &listItems)) {
            return false;
        }

        for (const auto &item : listItems) {
            extractPluginLib(item, i.key());
        }

    }

    return true;
}

bool Extracter::extractQmlFromSource() {

    auto cnf = DeployCore::_config;

    for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {
        auto targetPath = cnf->getTargetDir() + "/" + i.key();
        auto distro = cnf->getDistroFromPackage(i.key());

        QStringList plugins;
        QStringList listItems;
        QStringList filter;
        filter << ".so.debug" << "d.dll" << ".pdb";

        for (const auto &qmlInput: distro.qmlInput()) {
            QFileInfo info(qmlInput);

            if (!info.isDir()) {
                QuasarAppUtils::Params::log("extract qml fail! qml source dir not exits or is not dir " + qmlInput,
                                                   QuasarAppUtils::Error);
                continue;
            }
            QuasarAppUtils::Params::log("extractQmlFromSource " + info.absoluteFilePath());

            if (!QFileInfo::exists(cnf->qtDir.getQmls())) {
                QuasarAppUtils::Params::log("qml dir wrong!",
                                                   QuasarAppUtils::Warning);
                continue;
            }

            QML ownQmlScaner(cnf->qtDir.getQmls());

            if (!ownQmlScaner.scan(plugins, info.absoluteFilePath())) {
                QuasarAppUtils::Params::log("qml scaner run failed!",
                                                   QuasarAppUtils::Error);
                continue;
            }
        }

        if (!_fileManager->copyFolder(cnf->qtDir.getQmls(),
                                     targetPath + distro.getQmlOutDir(),
                        filter , &listItems, &plugins)) {
            return false;
        }

        for (const auto &item : listItems) {
            extractPluginLib(item, i.key());
        }

    }

    return true;
}

bool Extracter::extractQml() {

    if (QuasarAppUtils::Params::isEndable("qmlDir")) {
        return extractQmlFromSource();

    } else if (QuasarAppUtils::Params::isEndable("allQmlDependes")) {
        return extractQmlAll();

    }
    return false;
}

void Extracter::extract(const QString &file,
                        DependencyMap *depMap,
                        const QString &mask) {

    assert(depMap);

    QFileInfo info(file);

    auto sufix = info.completeSuffix();

    if (sufix.compare("dll", Qt::CaseSensitive) == 0 ||
            sufix.compare("exe", Qt::CaseSensitive) == 0 ||
            sufix.isEmpty() || sufix.contains("so", Qt::CaseSensitive)) {

        extractLib(file, depMap, mask);
    } else {
        QuasarAppUtils::Params::log("file with sufix " + sufix + " not supported!");
    }

}

Extracter::Extracter(FileManager *fileManager, ConfigParser *cqt,
                     DependenciesScanner *scaner):
    _scaner(scaner),
    _fileManager(fileManager),
    _cqt(cqt)
    {

    assert(_cqt);
    assert(_fileManager);
    assert(DeployCore::_config);

    _metaFileManager = new MetaFileManager(_fileManager);
}

