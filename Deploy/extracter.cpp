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
    qInfo () << "try deploy msvc";
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
            auto webEngeneBin = DeployCore::_config->qtDir.getLibexecs();
            if (DeployCore::_config->qtDir.getQtPlatform() & Platform::Unix) {
                webEngeneBin += "/QtWebEngineProcess";
            } else {
                webEngeneBin += "/QtWebEngineProcess.exe";
            }

            auto destWebEngine = DeployCore::_config->getTargetDir() + package + DeployCore::_config->packages()[package].getBinOutDir();
            auto resOut = DeployCore::_config->getTargetDir() + package + DeployCore::_config->packages()[package].getResOutDir();
            auto res = DeployCore::_config->qtDir.getResources();

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
    auto targetPath = cnf->getTargetDir() + package;
    auto distro = cnf->getDistroFromPackage(package);


    auto pluginPath = targetPath + distro.getPluginsOutDir() +
            QFileInfo(plugin).fileName();

    if (!_fileManager->copyFolder(plugin, pluginPath,
                    QStringList() << ".so.debug" << "d.dll", &listItems)) {
        return false;
    }

    for (const auto &item : listItems) {
        if (QuasarAppUtils::Params::isEndable("extractPlugins")) {
            extract(item, &_packageDependencyes[package]);
        } else {
            extract(item, &_packageDependencyes[package], "Qt");
        }
    }

    return true;
}

void Extracter::copyExtraPlugins(const QString& package) {
    QFileInfo info;

    auto cnf = DeployCore::_config;
    auto targetPath = cnf->getTargetDir() + package;
    auto distro = cnf->getDistroFromPackage(package);

    for (auto extraPlugin : DeployCore::_config->extraPlugins) {

        if (!PathUtils::isPath(extraPlugin)) {
            extraPlugin = DeployCore::_config->qtDir.getPlugins() + "/" + extraPlugin;
        }

        info.setFile(extraPlugin);
        if (info.isDir() && DeployCore::_config->qtDir.isQt(info.absoluteFilePath())) {
            copyPlugin(info.absoluteFilePath(), package);

        } else if (info.exists()) {
            _fileManager->copyFile(info.absoluteFilePath(),
                                  targetPath + distro.getPluginsOutDir());

            if (QuasarAppUtils::Params::isEndable("extractPlugins")) {
                extract(info.absoluteFilePath(), &_packageDependencyes[package]);
            } else {
                extract(info.absoluteFilePath(), &_packageDependencyes[package], "Qt");
            }
        }
    }
}

void Extracter::copyPlugins(const QStringList &list, const QString& package) {
    for (const auto &plugin : list) {
        if (!copyPlugin(plugin, package)) {
            qWarning() << plugin << " not copied!";
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
        qInfo() << "clear old data";

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
    auto targetPath = cnf->getTargetDir() + package;
    auto distro = cnf->getDistroFromPackage(package);

    for (const auto &file : files) {
        if (!_fileManager->smartCopyFile(file, targetPath + distro.getLibOutDir())) {
            QuasarAppUtils::Params::verboseLog(file + " not copied");
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
            QuasarAppUtils::Params::verboseLog("strip failed!");
        }
    }
}

void Extracter::copyTr() {

    if (!QuasarAppUtils::Params::isEndable("noTranslations")) {

        auto cnf = DeployCore::_config;

        for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {
            if (!copyTranslations(DeployCore::extractTranslation(_packageDependencyes[i.key()].neadedLibs()),
                                  i.key())) {
                QuasarAppUtils::Params::verboseLog("Failed to copy standard Qt translations",
                                                   QuasarAppUtils::Warning);
            }
        }

    }
}

void Extracter::deploy() {
    qInfo() << "target deploy started!!";

    clear();
    _cqt->smartMoveTargets();
    _scaner->setEnvironment(DeployCore::_config->envirement.deployEnvironment());
    extractAllTargets();

    if (DeployCore::_config->deployQml && !extractQml()) {
        qCritical() << "qml not extacted!";
    }

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

    auto targetPath = cnf->getTargetDir() + package;
    auto distro = cnf->getDistroFromPackage(package);

    for (const auto &i: listItems) {
        _fileManager->copyFile(i.absoluteFilePath(), targetPath + distro.getTrOutDir());
    }

    if (isWebEngine(package)) {
        auto trOut = targetPath + distro.getTrOutDir();
        auto tr = DeployCore::_config->qtDir.getTranslations() + "/qtwebengine_locales";
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

    qInfo() << "extract lib :" << file;

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

bool Extracter::extractQmlAll() {

    if (!QFileInfo::exists(DeployCore::_config->qtDir.getQmls())) {
        qWarning() << "qml dir wrong!";
        return false;
    }

    auto cnf = DeployCore::_config;


    for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {
        auto targetPath = cnf->getTargetDir() + i.key();
        auto distro = cnf->getDistroFromPackage(i.key());

        QStringList listItems;

        if (!_fileManager->copyFolder(cnf->qtDir.getQmls(), targetPath + distro.getQmlOutDir(),
                        QStringList() << ".so.debug" << "d.dll" << ".pdb",
                        &listItems)) {
            return false;
        }

        for (const auto &item : listItems) {
            if (QuasarAppUtils::Params::isEndable("extractPlugins")) {
                extract(item, &_packageDependencyes[i.key()]);
            } else {
                extract(item, &_packageDependencyes[i.key()], "Qt");
            }
        }

    }

    return true;
}

bool Extracter::extractQmlFromSource() {

    auto cnf = DeployCore::_config;

    for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {
        auto targetPath = cnf->getTargetDir() + i.key();
        auto distro = cnf->getDistroFromPackage(i.key());

        QStringList plugins;
        QStringList listItems;
        QStringList filter;
        filter << ".so.debug" << "d.dll" << ".pdb";

        for (const auto &qmlInput: distro.qmlInput()) {
            QFileInfo info(qmlInput);

            if (!info.isDir()) {
                qCritical() << "extract qml fail! qml source dir not exits or is not dir " << qmlInput;
                continue;
            }
            QuasarAppUtils::Params::verboseLog("extractQmlFromSource " + info.absoluteFilePath());

            if (!QFileInfo::exists(cnf->qtDir.getQmls())) {
                qWarning() << "qml dir wrong!";
                continue;
            }

            QML ownQmlScaner(cnf->qtDir.getQmls());

            if (!ownQmlScaner.scan(plugins, info.absoluteFilePath())) {
                QuasarAppUtils::Params::verboseLog("qml scaner run failed!",
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
            if (QuasarAppUtils::Params::isEndable("extractPlugins")) {
                extract(item, &_packageDependencyes[i.key()]);
            } else {
                extract(item, &_packageDependencyes[i.key()], "Qt");
            }
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
        QuasarAppUtils::Params::verboseLog("file with sufix " + sufix + " not supported!");
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

