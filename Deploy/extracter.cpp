/*
 * Copyright (C) 2018-2021 QuasarApp.
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
    return static_cast<quint64>(qtModules) & static_cast<quint64>(DeployCore::QtModule::QtWebEngineCoreModule);
}

bool Extracter::extractWebEngine() {

    auto cnf = DeployCore::_config;

    for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {
        const auto &package = i.key();

        if (isWebEngine(package)) {
            auto webEngeneBin = cnf->qtDir.getLibexecs();

            if (cnf->qtDir.getQtPlatform() & Platform::Unix) {
                webEngeneBin += "/QtWebEngineProcess";
            } else if (cnf->qtDir.getQtPlatform() & Platform::Win) {
                webEngeneBin += "/QtWebEngineProcess.exe";
            }

            auto destWebEngine = cnf->getTargetDir() + "/" + package + cnf->getDistroFromPackage(package).getBinOutDir();
            auto resOut = cnf->getTargetDir() + "/" + package + cnf->getDistroFromPackage(package).getResOutDir();
            auto libOut = cnf->getTargetDir() + "/" + package + cnf->getDistroFromPackage(package).getLibOutDir();

            auto res = cnf->qtDir.getResources();
            if (!_fileManager->copyFiles(angleGLLibs(), libOut)) {
                return false;
            }

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

QList<QString> Extracter::angleGLLibs() {
    auto cnf = DeployCore::_config;

    if (cnf->qtDir.getQtPlatform() & Platform::Win) {
        return {
            cnf->qtDir.getBins() + "/d3dcompiler_47.dll",
                    cnf->qtDir.getBins() + "/libEGL.dll",
                    cnf->qtDir.getBins() + "/libGLESv2.dll",
        };
    }

    return {};

}

void Extracter::extractAllTargets() {
    auto cfg = DeployCore::_config;
    for (auto i = cfg->packages().cbegin(); i != cfg->packages().cend(); ++i) {
        auto &dep = _packageDependencyes[i.key()];

        for (const auto &target : i.value().targets()) {
            extract(target, &dep);
        }
    }
}

void Extracter::extractExtraDataTargets() {
    auto cfg = DeployCore::_config;
    for (auto i = cfg->packages().cbegin(); i != cfg->packages().cend(); ++i) {
        auto &dep = _packageDependencyes[i.key()];
        const auto extraData = i.value().extraData();
        for (const auto &target : extraData) {
            dep.addExtraData(target);
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

void Extracter::copyExtraPlugins(const QString& package) {


    QFileInfo info;

    auto cnf = DeployCore::_config;
    auto targetPath = cnf->getTargetDir() + "/" + package;
    auto distro = cnf->getDistroFromPackage(package);
    const auto plugins = distro.extraPlugins();

    for (const auto &extraPlugin : plugins) {

        info.setFile(extraPlugin);

        if (info.isFile()) {
            if (!_fileManager->copyFile(info.absoluteFilePath(),
                                        targetPath + distro.getPluginsOutDir())) {

                QuasarAppUtils::Params::log("fail to copy extra plugin from:" + info.absoluteFilePath() +
                                            " to: " + targetPath + distro.getPluginsOutDir(),
                                            QuasarAppUtils::Warning);
            }

            extractPluginLib(info.absoluteFilePath(), package);
            continue;
        }

        if (info.isDir()) {
            QStringList plugins;
            if (!_fileManager->copyFolder(info.absoluteFilePath(),
                                          targetPath + distro.getPluginsOutDir() + info.fileName(),
                                          DeployCore::debugExtensions(),
                                          &plugins)) {

                QuasarAppUtils::Params::log("fail to copy extra plugin from:" + info.absoluteFilePath() +
                                            " to: " + targetPath + distro.getPluginsOutDir(),
                                            QuasarAppUtils::Warning);
            }

            for (const auto& plugin : qAsConst(plugins)) {
                extractPluginLib(plugin, package);
            }
        }
    }
}

void Extracter::extractPlugins() {
    auto cnf = DeployCore::_config;

    _pluginsParser->initDeployPluginsList();

    for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {
        auto targetPath = cnf->getTargetDir() + "/" + i.key();
        auto distro = cnf->getDistroFromPackage(i.key());

        QStringList plugins;
        QStringList listItems;

        _pluginsParser->scan(cnf->qtDir.getPlugins(), plugins, _packageDependencyes[i.key()].qtModules(), i.key());

        _fileManager->copyFiles(plugins, targetPath + distro.getPluginsOutDir(), 1,
                                DeployCore::debugExtensions(), &listItems);

        for (const auto &item : qAsConst(listItems)) {
            extractPluginLib(item, i.key());
        }

        copyExtraPlugins(i.key());
    }

}

void Extracter::copyLibs(const QSet<QString> &files, const QString& package, bool system) {
    auto cnf = DeployCore::_config;
    auto targetPath = cnf->getTargetDir() + "/" + package;
    auto distro = cnf->getDistroFromPackage(package);

    auto libOutpath = targetPath + distro.getLibOutDir();
    if (system) {
        libOutpath += "/" + DeployCore::systemLibsFolderName();
    }

    for (const auto &file : files) {
        if (!_fileManager->smartCopyFile(file, libOutpath)) {
            QuasarAppUtils::Params::log(file + " not copied");
        }
    }
}

void Extracter::copyExtraData(const QSet<QString> &files, const QString &package) {
    auto cnf = DeployCore::_config;
    auto targetPath = cnf->getTargetDir() + "/" + package;
    auto distro = cnf->getDistroFromPackage(package);

    for (const auto &file : files) {

        if (!_fileManager->cp(file, targetPath + distro.getExtraDataOutDir())) {
            QuasarAppUtils::Params::log(file + " not copied");
        }
    }
}

void Extracter::copyFiles() {
    auto cnf = DeployCore::_config;

    for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {

        copyLibs(_packageDependencyes[i.key()].neadedLibs(), i.key(), false);

        if (QuasarAppUtils::Params::isEndable("deploySystem")) {
            copyLibs(_packageDependencyes[i.key()].systemLibs(), i.key(), true);
        }


        if (!QuasarAppUtils::Params::isEndable("noStrip") && !_fileManager->strip(cnf->getTargetDir())) {
            QuasarAppUtils::Params::log("strip failed!");
        }

        copyExtraData(_packageDependencyes[i.key()].extraData(), i.key());
    }
}

bool Extracter::copyTr() {

    if (!QuasarAppUtils::Params::isEndable("noTranslations")) {

        auto cnf = DeployCore::_config;
        for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {
            if (!copyTranslations(DeployCore::extractTranslation(_packageDependencyes[i.key()].neadedLibs()),
                                  i.key())) {
                QuasarAppUtils::Params::log("Failed to copy standard Qt translations",
                                            QuasarAppUtils::Warning);
            }

            const auto trFiles =  i->tr();
            for (const auto &tr: trFiles) {
                if (!_fileManager->copyFile(tr, cnf->getPackageTargetDir(i.key()) + i->getTrOutDir())) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool Extracter::deploy() {
    QuasarAppUtils::Params::log("target deploy started!!",
                                QuasarAppUtils::Info);
    if (!_cqt->smartMoveTargets()) {
        QuasarAppUtils::Params::log("Fail to copy targets", QuasarAppUtils::Error);
        return false;
    }

    _scaner->setEnvironment(DeployCore::_config->envirement.environmentList());
    extractAllTargets();
    extractExtraDataTargets();

    if (DeployCore::_config->deployQml && !extractQml()) {
        QuasarAppUtils::Params::log("qml not extacted!",
                                    QuasarAppUtils::Error);
    }

    extractPlugins();

    copyFiles();

    if (!copyTr()) {
        QuasarAppUtils::Params::log("Fail to copy translations", QuasarAppUtils::Error);

        return false;
    };

    if (!extractWebEngine()) {
        QuasarAppUtils::Params::log("deploy webEngine failed", QuasarAppUtils::Error);
        return false;
    }

    if (!deployMSVC()) {
        QuasarAppUtils::Params::log("deploy msvc failed", QuasarAppUtils::Warning);
    }

    _metaFileManager->createRunMetaFiles();
    QuasarAppUtils::Params::log("deploy done!",
                                QuasarAppUtils::Info);

    return true;

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
                                QuasarAppUtils::Debug);

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
    extract(item, &_packageDependencyes[package]);

}

bool Extracter::extractQml() {

    if (QuasarAppUtils::Params::isEndable("qmlDir")) {
        auto cnf = DeployCore::_config;

        for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {
            auto targetPath = cnf->getTargetDir() + "/" + i.key();
            auto distro = cnf->getDistroFromPackage(i.key());

            QStringList plugins;
            QStringList listItems;
            const auto qmlInput = distro.qmlInput();
            for (const auto &qmlInput: qmlInput) {
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
                                          DeployCore::debugExtensions() ,
                                          &listItems, &plugins)) {
                return false;
            }

            for (const auto &item : qAsConst(listItems)) {
                extractPluginLib(item, i.key());
            }

        }

        return true;
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

Extracter::Extracter(FileManager *fileManager, PluginsParser *pluginsParser, ConfigParser *cqt,
                     DependenciesScanner *scaner):
    _scaner(scaner),
    _fileManager(fileManager),
    _pluginsParser(pluginsParser),
    _cqt(cqt)
{

    assert(_cqt);
    assert(_fileManager);
    assert(_pluginsParser);
    assert(DeployCore::_config);

    _metaFileManager = new MetaFileManager(_fileManager);
}

