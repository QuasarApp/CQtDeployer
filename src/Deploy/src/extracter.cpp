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
#include <quasarapp.h>
#include <cstdio>
#include <qaglobalutils.h>
#include <cassert>

#include <fstream>

#include <LIEF/MachO.hpp>

namespace {

/**
 * @brief pathRelative This function return the relative path from the
 *  directory @a fromDir to the file or directory @a to (POSIX style).
 *  The result is "." if both paths are equal.
 */
static QString pathRelative(const QString &fromDir, const QString &to) {
    auto fromList = fromDir.split('/', Qt::SkipEmptyParts);
    auto toList = to.split('/', Qt::SkipEmptyParts);

    int common = 0;
    while (common < fromList.size() && common < toList.size()
            && fromList.at(common) == toList.at(common)) {
        ++common;
    }

    QStringList up;
    for (int i = common; i < fromList.size(); ++i) {
        up << "..";
    }

    auto rest = toList.mid(common);

    if (up.isEmpty()) {
        return rest.isEmpty() ? QStringLiteral(".") : rest.join("/");
    }

    if (rest.isEmpty()) {
        return up.join("/");
    }

    return up.join("/") + "/" + rest.join("/");
}

} // namespace

bool Extracter::deployMSVC() {
    QuasarAppUtils::Params::log("Trying to deploy msvc", QuasarAppUtils::Debug);
    auto msvcInstaller = DeployCore::getVCredist(DeployCore::_config->qtDir.getBins());

    if (msvcInstaller.isEmpty()) {
        return false;
    }

    return _fileManager->copyFile(msvcInstaller, DeployCore::_config->getTargetDir());
}

bool Extracter::isMachOFile(const QString &file) {
    return DeployCore::isMacOBinary(QFileInfo(file));
}

QStringList Extracter::machORPaths(const QString &file) {
    // The implementation lives in DeployCore::machORPaths(), because the
    // rpaths are also needed by the scanner (macho_type.cpp), which has no
    // access to the extracter (issue #166).
    return DeployCore::machORPaths(file);
}

Extracter::MacOFileChanges Extracter::planMacOChanges(
        const QString &file,
        const QHash<QString, QString> &references,
        const QString &libOutDir) {
    MacOFileChanges changes;

    auto macho = LIEF::MachO::Parser::parse(file.toStdString());
    if (macho == nullptr || macho->empty()) {
        return changes;
    }

    QSet<QString> seenChanges;

    // The LC_RPATH entries are read directly from the binary, because the
    // LIEF submodule of CQtDeployer maps LC_RPATH to a different load
    // command constant than the macOS ABI (0x1C01), so the LIEF rpaths()
    // accessor would not find the real entries (see machORPaths).
    // The LC_RPATH strings are compared after stripping the trailing
    // slashes, because install_name_tool treats "@executable_path/.." and
    // "@executable_path/../" as the same path, while the raw strings stored
    // in the binary can differ (issue #166).
    auto normalizeRPath = [](QString rpath) {
        while (rpath.endsWith('/')) {
            rpath.chop(1);
        }
        return rpath;
    };

    QSet<QString> rpaths;
    for (auto rpath : machORPaths(file)) {
        rpaths.insert(normalizeRPath(rpath));
    }

    // A fat (universal) binary contains several architecture slices, so
    // the changes are collected from all of them (deduplicated by name).
    for (const LIEF::MachO::Binary &slice : *macho) {

        for (const LIEF::MachO::DylibCommand &dylib : slice.libraries()) {
            auto name = QString::fromStdString(dylib.name());
            if (name.isEmpty()) {
                continue;
            }

            auto replacement = references.value(
                        DeployCore::normalizeMacOSLibName(name).toUpper());
            if (replacement.isEmpty() || replacement == name) {
                continue;
            }

            auto key = name + "->" + replacement;
            if (seenChanges.contains(key)) {
                continue;
            }

            seenChanges.insert(key);
            changes.changedLibs.append(qMakePair(name, replacement));
        }

        if (auto *idCmd = slice.get(LIEF::MachO::LoadCommand::TYPE::ID_DYLIB);
                idCmd != nullptr) {
            if (auto *id = idCmd->cast<LIEF::MachO::DylibCommand>()) {
                auto oldId = QString::fromStdString(id->name());
                auto newId = references.value(
                            DeployCore::normalizeMacOSLibName(file).toUpper());
                if (!newId.isEmpty() && newId != oldId) {
                    changes.newInstallName = newId;
                }
            }
        }
    }

    // Make sure the deployed libraries can be resolved: add an rpath that
    // points from the binary to the deployed lib directory.
    auto relPath = pathRelative(QFileInfo(file).absolutePath(), libOutDir);
    if (!relPath.isEmpty()) {
        auto rpath = (relPath == ".")
                ? QStringLiteral("@executable_path")
                : QStringLiteral("@executable_path/") + relPath;

        if (!rpaths.contains(normalizeRPath(rpath))) {
            changes.addedRPaths << rpath;
        }
    }

    return changes;
}

bool Extracter::fixMachOWithInstallNameTool(
        const QString &file, const MacOFileChanges &changes) {
    QProcess proc;

    auto runTool = [&](const QString &program, const QStringList &args,
                       const QString &okIfErrorContains = QString()) -> bool {
        auto commandArgs = args;
        commandArgs << file;
        proc.start(program, commandArgs);

        if (!proc.waitForStarted() || !proc.waitForFinished()) {
            QuasarAppUtils::Params::log(program + " failed to start: " + file,
                                        QuasarAppUtils::Warning);
            return false;
        }

        if (proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
            auto error = QString::fromLocal8Bit(proc.readAllStandardError()).trimmed();

            // An expected error (for example adding an rpath that already
            // exists) is not a failure.
            if (!okIfErrorContains.isEmpty() && error.contains(okIfErrorContains)) {
                QuasarAppUtils::Params::log(program + ": " + error,
                                            QuasarAppUtils::Debug);
                return true;
            }

            QuasarAppUtils::Params::log(program + " failed: " + error,
                                        QuasarAppUtils::Warning);
            return false;
        }

        return true;
    };

    bool ok = true;

    for (const auto &change : std::as_const(changes.changedLibs)) {
        if (!runTool("/usr/bin/install_name_tool",
                     {"-change", change.first, change.second})) {
            ok = false;
        }
    }

    if (!changes.newInstallName.isEmpty()
            && !runTool("/usr/bin/install_name_tool", {"-id", changes.newInstallName})) {
        ok = false;
    }

    for (const auto &rpath : std::as_const(changes.addedRPaths)) {
        if (!runTool("/usr/bin/install_name_tool", {"-add_rpath", rpath},
                     QStringLiteral("already has LC_RPATH"))) {
            ok = false;
        }
    }

    // install_name_tool invalidates the code signature, so the binary has
    // to be signed again. An ad-hoc signature is enough for a local
    // deployment.
    if (ok && !runTool("/usr/bin/codesign", {"--force", "--sign", "-"})) {
        ok = false;
    }

    return ok;
}

bool Extracter::deployMacOSRPaths() {

    auto cnf = DeployCore::_config;
    bool ok = true;

    for (auto i = cnf->packages().cbegin(); i != cnf->packages().cend(); ++i) {
        const auto &package = i.key();
        auto targetPath = cnf->getTargetDir() + "/" + package;
        auto distro = cnf->getDistroFromPackage(package);
        auto libOut = targetPath + distro.getLibOutDir();

        // Build the map of the normalized library names to the dyld
        // references that must exist in the deployed layout.
        QHash<QString, QString> references;

        auto addReference = [&references](const QString &sourceLib,
                                          const QString &deployedRel) {
            auto ref = "@rpath/" + deployedRel;
            references.insert(DeployCore::normalizeMacOSLibName(sourceLib).toUpper(), ref);
            references.insert(QFileInfo(sourceLib).fileName().toUpper(), ref);
        };

        for (const auto &lib : _packageDependencyes[package].neadedLibs()) {
            addReference(lib, DeployCore::macOSFrameworkReference(lib));
        }

        if (QuasarAppUtils::Params::isEndable("deploySystem")) {
            for (const auto &lib : _packageDependencyes[package].systemLibs()) {
                addReference(lib, DeployCore::systemLibsFolderName() + "/" +
                          QFileInfo(lib).fileName());
            }
        }

        QDirIterator it(targetPath, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            auto file = it.next();
            if (!isMachOFile(file)) {
                continue;
            }

            auto changes = planMacOChanges(file, references, libOut);
            if (changes.changedLibs.isEmpty() && changes.newInstallName.isEmpty()
                    && changes.addedRPaths.isEmpty()) {
                continue;
            }

            if (!fixMachOWithInstallNameTool(file, changes)) {
                ok = false;
            }
        }
    }

    return ok;
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

            if (cnf->qtDir.getQtPlatform() & (Platform::Unix | Platform::Mac)) {
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

    // macOS: rewrite the Mach-O install names and rpaths of the deployed
    // binaries so that the libraries are resolved from the deployed
    // directory (issue #166).
    if (!deployMacOSRPaths()) {
        QuasarAppUtils::Params::log("Failed to fix the Mach-O install names and rpaths",
                                    QuasarAppUtils::Warning);
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
            }


            for (const auto &item : std::as_const(listItems)) {
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
