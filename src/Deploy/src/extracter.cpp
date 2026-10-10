/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "extracter.h"
#include "deploycore.h"
#include "pluginsparser.h"
#include "configparser.h"
#include "metafilemanager.h"
#include "qmlqt5.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>
#include <QSet>
#include <quasarapp.h>
#include <cstdio>
#include <qaglobalutils.h>
#include <cassert>

#include <fstream>

bool Extracter::deployMSVC() {
    QuasarAppUtils::Params::log("Trying to deploy msvc", QuasarAppUtils::Debug);
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

        for (const auto &targetId : i.value().targets()) {
            extract(targetId, &dep);
        }
    }
}

void Extracter::extractExtraDataTargets() {
    auto cfg = DeployCore::_config;
    for (auto i = cfg->packages().cbegin(); i != cfg->packages().cend(); ++i) {
        auto &dep = _packageDependencyes[i.key()];
        const auto extraData = i.value().extraData();
        for (const auto &target : extraData) {

            QFileInfo info = DeployCore::findItem(target);
            if (!info.exists()) {
                QuasarAppUtils::Params::log("Failed to copy extra data from: " + target +
                                            " Error: target not exists!.", QuasarAppUtils::Warning);
                continue;
            }

            dep.addExtraData(info.absoluteFilePath());
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

                QuasarAppUtils::Params::log("Failed to copy extra plugin from:" + info.absoluteFilePath() +
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

                QuasarAppUtils::Params::log("Failed to copy extra plugin from:" + info.absoluteFilePath() +
                                            " to: " + targetPath + distro.getPluginsOutDir(),
                                            QuasarAppUtils::Warning);
            }

            for (const auto& plugin : std::as_const(plugins)) {
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

        for (const auto &item : std::as_const(listItems)) {
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
        _fileManager->smartCopyFile(file, libOutpath);
    }
}

void Extracter::copyExtraData(const QSet<QString> &files, const QString &package) {
    auto cnf = DeployCore::_config;
    auto targetPath = cnf->getTargetDir() + "/" + package;
    auto distro = cnf->getDistroFromPackage(package);

    for (const auto &file : files) {

        if (!_fileManager->cp(file, targetPath + distro.getExtraDataOutDir())) {
            QuasarAppUtils::Params::log("Failed to copy " + file);
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
            QuasarAppUtils::Params::log("Failed to strip libraries!");
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

            const auto trFiles =  i->translation();
            for (const auto &tr: trFiles) {

                QFileInfo info(tr);

                if (!info.exists()) {
                    QuasarAppUtils::Params::log("Failed to copy " + info.absoluteFilePath() + ". Not exists",
                                                QuasarAppUtils::Warning);
                    continue;
                }

                if (info.isDir()) {
                    QDir dir(info.absoluteFilePath());
                    auto availableQm = dir.entryInfoList({"*.qm"}, QDir::Files);
                    for (const auto & trFile : std::as_const(availableQm)) {
                        if (!_fileManager->copyFile(trFile.absoluteFilePath(),
                                                    cnf->getPackageTargetDir(i.key()) + i->getTrOutDir())) {
                            return false;
                        }
                    }
                } else {
                    if (!_fileManager->copyFile(tr, cnf->getPackageTargetDir(i.key()) + i->getTrOutDir())) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool Extracter::deploy() {
    QuasarAppUtils::Params::log("target deploy started!!",
                                QuasarAppUtils::Debug);
    if (!_cqt->smartMoveTargets()) {
        QuasarAppUtils::Params::log("Fail to copy targets", QuasarAppUtils::Error);
        return false;
    }

    _scaner->setEnvironment(DeployCore::_config->envirement.environmentList());
    extractAllTargets();
    extractExtraDataTargets();

    if (DeployCore::_config->deployQml && !extractQml()) {
        QuasarAppUtils::Params::log("Failed to extract qml!",
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
        QuasarAppUtils::Params::log("Failed to deploy msvc", QuasarAppUtils::Debug);
    }

    _metaFileManager->createRunMetaFiles(_targetModules);
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

    auto listItems = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);

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

    auto list = dir.entryInfoList( QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);

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

    QSet<LibInfo> allDependencies;
    auto targetObject = DeployCore::_config->targets().value(file);
    if (targetObject.isValid()) {
        _scaner->scan(targetObject);
        allDependencies = targetObject.getAllDep();
    } else {
        allDependencies = _scaner->scan(file).getAllDep();
    }

    for (const auto &line : std::as_const(allDependencies)) {

        if (mask.size() && !line.getName().contains(mask, DeployCore::getCaseSensitivity())) {
            continue;
        }

        if (DeployCore::_config->ignoreList.isIgnore(line)) {
            continue;
        }

        _targetModules[file] = _targetModules.value(file, DeployCore::NONE) | DeployCore::getQtModule(line.fullPath());

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
            QStringList qmlInputDirs;
            auto QtVersion = cnf->isNeededQt(i.key());

            const auto qmlInputList = distro.qmlInput();
            for (const auto &qmlInput: qmlInputList) {
                QFileInfo info(qmlInput);

                if (!info.isDir()) {
                    QuasarAppUtils::Params::log("Failed to extract qml! The qml source dir does not exist :" + qmlInput,
                                                QuasarAppUtils::Error);
                    continue;
                }
                QuasarAppUtils::Params::log("extractQmlFromSource " + info.absoluteFilePath());

                if (!QFileInfo::exists(cnf->qtDir.getQmls())) {
                    QuasarAppUtils::Params::log("Failed to extract qml! The qt qml dir is not initialized!",
                                                QuasarAppUtils::Warning);
                    continue;
                }
                qmlInputDirs.push_back(info.absoluteFilePath());

                QSharedPointer<iQML> qmlScaner;
                if (QtVersion & QtMajorVersion::Qt6) {
                    qmlScaner = QSharedPointer<QMLQt6>::create(cnf->qtDir.getQmls());
                } else if (QtVersion & QtMajorVersion::Qt5) {
                    qmlScaner = QSharedPointer<QMLQt5>::create(cnf->qtDir.getQmls());
                }

                if (qmlScaner && !qmlScaner->scan(plugins, info.absoluteFilePath())) {
                    QuasarAppUtils::Params::log("Failed to run qml scanner",
                                                QuasarAppUtils::Error);
                    continue;
                }

            }

            QStringList toCopyQmlFiles;
            if (QtVersion & QtMajorVersion::Qt5) {
                // See the https://github.com/QuasarApp/CQtDeployer/issues/728 issue
                // use old method of parse qml for qt5
                if (!_fileManager->copyFolder(cnf->qtDir.getQmls(),
                                              targetPath + distro.getQmlOutDir(),
                                              DeployCore::debugExtensions() ,
                                              &listItems, &plugins)) {
                    return false;
                }
            } else {
                for (const auto& plugin: std::as_const(plugins)) {
                    const auto qmlFiles = QDir(plugin).entryInfoList(QDir::Files | QDir::Hidden);
                    for (const auto& qmlFile: qmlFiles) {
                        toCopyQmlFiles.push_back(qmlFile.absoluteFilePath());
                    }
                }

                // This function works very slow because use list mask
                // solution: use the QSet and restriction comparese of the pathes for the mask argument.
                // to-do optimise this function
                if (!_fileManager->copyFolder(cnf->qtDir.getQmls(),
                                              targetPath + distro.getQmlOutDir(),
                                              DeployCore::debugExtensions() ,
                                              &listItems,
                                              &toCopyQmlFiles)) {
                    return false;
                }

                // Custom auto generated qml modules (created by qt_add_qml_module without
                // the NO_PLUGIN option) are built into the project build dir and are not
                // part of the Qt distribution. Deploy them together with their plugins.
                // See the https://github.com/QuasarApp/CQtDeployer/issues/847 issue.
                const auto qtQmlRoot = QFileInfo(cnf->qtDir.getQmls()).absoluteFilePath();
                QSet<QString> deployedModules;
                for (const auto& plugin: std::as_const(plugins)) {
                    const auto absPlugin = QFileInfo(plugin).absoluteFilePath();
                    if (absPlugin.isEmpty() ||
                        absPlugin == qtQmlRoot ||
                        absPlugin.startsWith(qtQmlRoot + QDir::separator())) {
                        continue; // qt distribution module, already handled above
                    }
                    if (deployedModules.contains(absPlugin)) {
                        continue; // already deployed
                    }
                    deployedModules.insert(absPlugin);

                    // The deploy subdir is the module uri (from the qmldir "module"
                    // line), so that the module is deployed next to its import path.
                    QString relPath;
                    const auto qmldir = QFileInfo(absPlugin + QDir::separator() + "qmldir");
                    if (qmldir.exists()) {
                        const auto uri = QMLQt6::extractModuleName(qmldir.absoluteFilePath());
                        if (!uri.isEmpty()) {
                            relPath = uri;
                            relPath.replace(".", "/");
                        }
                    }

                    // Fallback: use the relative path from the qml source dir,
                    // or the dir name, if the module uri cannot be read.
                    if (relPath.isEmpty()) {
                        for (const auto& qmlInputDir: std::as_const(qmlInputDirs)) {
                            const auto absInput = QFileInfo(qmlInputDir).absoluteFilePath();
                            const auto prefix = absInput + QDir::separator();
                            if (absPlugin.startsWith(prefix)) {
                                relPath = absPlugin.mid(prefix.size());
                                break;
                            }
                        }
                    }
                    if (relPath.isEmpty()) {
                        relPath = QFileInfo(absPlugin).fileName();
                    }

                    if (relPath.isEmpty()) {
                        QuasarAppUtils::Params::log("Failed to deploy custom qml module: " + absPlugin,
                                                    QuasarAppUtils::Warning);
                        continue;
                    }

                    const auto dest = targetPath + distro.getQmlOutDir() + QDir::separator() + relPath;
                    if (!copyQmlModuleFiles(absPlugin, dest, &listItems)) {
                        QuasarAppUtils::Params::log("Failed to copy custom qml module from: " +
                                                    absPlugin + " to: " + dest,
                                                    QuasarAppUtils::Warning);
                    }
                }
            }


            for (const auto &item : std::as_const(listItems)) {
                extractPluginLib(item, i.key());
            }

        }

        return true;
    }

    return false;
}

bool Extracter::copyQmlModuleFiles(const QString &fromDir, const QString &toDir, QStringList *listItems) {
    // A custom qml module (created by qt_add_qml_module without the NO_PLUGIN
    // option) is built into the project build dir together with the cmake build
    // artifacts (Makefile, *.cpp, CMakeFiles, ...). Only the qml module files
    // are needed for the deployment, so copy only the top level qml module
    // files (and not the cmake artifacts).
    // See the https://github.com/QuasarApp/CQtDeployer/issues/847 issue.
    static const QStringList qmlModuleExtensions = {
        "qml", "so", "qmltypes", "js", "json",
        "png", "jpg", "jpeg", "svg", "gif", "bmp", "ico",
        "ttf", "otf", "wav", "mp3"
    };

    QDir dir(fromDir);
    if (!dir.exists() || !dir.isReadable()) {
        return false;
    }

    if (!_fileManager->initDir(toDir)) {
        return false;
    }

    const auto files = dir.entryInfoList(QDir::Files | QDir::Hidden);
    for (const auto &file : files) {
        const auto name = file.fileName();

        const bool isQmldir = (name.compare("qmldir", Qt::CaseSensitive) == 0);
        if (!isQmldir) {
            const auto ext = QFileInfo(name).suffix();
            if (!qmlModuleExtensions.contains(ext, Qt::CaseInsensitive)) {
                continue; // skip cmake build artifacts and other non qml files
            }
        }

        if (!_fileManager->copyFile(file.absoluteFilePath(), toDir)) {
            QuasarAppUtils::Params::log("not copied qml module file " + toDir + "/" + name,
                                        QuasarAppUtils::Warning);
            continue;
        }

        if (listItems) {
            *listItems << toDir + "/" + name;
        }
    }

    return true;
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
