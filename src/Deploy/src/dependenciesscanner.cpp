/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "dependenciesscanner.h"
#include "deployconfig.h"
#include "deploycore.h"
#include "qaglobalutils.h"
#include <QList>
#include <QDir>
#include <QDebug>
#include <algorithm>

DependenciesScanner::DependenciesScanner() {

}

void DependenciesScanner::clearScaned() {
    _scanedLibs.clear();
}

PrivateScaner DependenciesScanner::getScaner(const QString &lib) const {

    QFileInfo info(lib);

    auto sufix = info.completeSuffix();

    if (sufix.contains("dll", Qt::CaseSensitive)  ||
            sufix.contains("exe", Qt::CaseSensitive)) {
        return PrivateScaner::PE;
    } else if (sufix.isEmpty() || sufix.contains("so", Qt::CaseSensitive)) {
        // macOS framework binaries have no file suffix, so they must be
        // checked before assuming an ELF library (issue #166).
        if (DeployCore::isMacOSFrameworkBinary(info)) {
            return PrivateScaner::MACOS;
        }

        // macOS executables and dylibs without a file suffix are Mach-O
        // binaries, but the same layout (no suffix) is used by the Linux
        // executables and the ELF cross-deployed from macOS. The full file
        // parsing on the LIEF backend separates both formats (issue #166).
        if (DeployCore::isMacOSBinary(info)) {
            return PrivateScaner::MACOS;
        }

        return PrivateScaner::ELF;
    } else if (sufix.contains("dylib", Qt::CaseSensitive)) {
        return PrivateScaner::MACOS;
    }

    return PrivateScaner::UNKNOWN;
}

QMultiMap<LibPriority, LibInfo> DependenciesScanner::getLibsFromEnvirement(
        const QString &libName) const {

    auto values = _EnvLibs.values(libName.toUpper());

    if (values.isEmpty()) {
        // macOS dependencies are Mach-O install names (@rpath/...,
        // /opt/homebrew/..., Name.framework/...) and do not match the
        // plain file names that are registered in the environment, so
        // the normalized name is tried as a fallback (issue #166).
        auto normalized = DeployCore::normalizeMacOSLibName(libName).toUpper();
        if (!normalized.isEmpty() && normalized != libName.toUpper()) {
            values = _EnvLibs.values(normalized);
        }
    }

    QMultiMap<LibPriority, LibInfo> res;

    struct Candidate {
        LibPriority _priority;
        LibInfo _info;
    };

    QList<Candidate> candidates;

    for (const auto & lib : values) {
        LibInfo info;

        if (_scanedLibs.contains(lib)) {
            info = _scanedLibs.value(lib);

            if ((info._priority >= SystemLib) && !QuasarAppUtils::Params::isEndable("deploySystem")) {
                continue;
            }

        } else {

            auto priority = (DeployCore::getLibPriority(lib));

            if ((priority >= SystemLib) && !QuasarAppUtils::Params::isEndable("deploySystem")) {
                continue;
            }

            info.setPriority(priority);

            if (!fillLibInfo(info, lib)) {
                qWarning() << "Failed to extract lib info from " + lib + "(" + libName + ")";
                continue;
            }
        }

        if (!DeployCore::_config->ignoreList.isIgnore(info)) {
            candidates.append(Candidate{info.getPriority(), info});
        }

    }

    // The scan environment is a QHash and its iteration order is salted per
    // process, so without a stable order every cqtdeployer process could
    // pick a different copy of a library that exists in several directories
    // (for example an old libstdc++-6.dll instead of the one from the build
    // toolchain, issue #861, #862). Sort by priority and then by full path
    // to make the selection deterministic.
    std::sort(candidates.begin(), candidates.end(), [](const auto &a, const auto &b) {
        if (a._priority != b._priority) {
            return a._priority < b._priority;
        }
        return a._info.fullPath() < b._info.fullPath();
    });

    for (const auto &candidate : candidates) {
        res.insert(candidate._priority, candidate._info);
    }

    return res;
}

bool DependenciesScanner::fillLibInfo(LibInfo &info, const QString &file) const {

    info.clear();
    auto scaner = getScaner(file);

    switch (scaner) {
    case PrivateScaner::PE: {
        return _peScaner.getLibInfo(file, info);
    }

    case PrivateScaner::ELF: {
        return _elfScaner.getLibInfo(file, info);
    }

    case PrivateScaner::MACOS: {
        return _macOSScaner.getLibInfo(file, info);
    }

    default:
        return _filesScaner.getLibInfo(file, info);
    }
}

void DependenciesScanner::recursiveDep(LibInfo &lib, QSet<LibInfo> &res, QSet<QString>& libStack) {

    qWarning() << "Get the recursive dependencies of " + lib.fullPath();

    if (_scanedLibs.contains(lib.fullPath())) {
        auto scanedLib = _scanedLibs.value(lib.fullPath());

        if (!scanedLib.isValid()) {
            qCritical() << "Detected an invalid library in scanned library cache!!";
            return;
        }

        res.unite(scanedLib._allDep);

        return;
    }

    if (libStack.contains(lib.fullPath())) {

        qWarning() << "A recursive dependency was found in library " + lib.fullPath();
        return;
    }

    libStack.insert(lib.fullPath());

    for (const auto &i : std::as_const(lib._dependncies)) {

        auto libs = getLibsFromEnvirement(i);

        if (!libs.size()) {
            qDebug() << "Cannot find the library for dependency " + i;
            continue;
        }

        auto dep = libs.begin();

        while (dep != libs.end() &&
               !DeployCore::isPlatformCompatible(dep.value()._platform, lib._platform)) dep++;

        if (dep != libs.end() && !res.contains(*dep)) {
            res.insert(*dep);

            LibInfo scanedLib = _scanedLibs.value(dep->fullPath());

            if (!scanedLib.isValid()) {
                QSet<LibInfo> listDep =  {};

                if (!lib._name.compare(dep.value()._name, DeployCore::getCaseSensitivity(lib._name)))
                    continue;

                recursiveDep(*dep, listDep, libStack);

                dep->_allDep = listDep;
                lib.setWinApi(lib.getWinApi() | dep->getWinApi());
                _scanedLibs.insert(dep->fullPath(), *dep);

                res.unite(listDep);
            } else {
                lib.setWinApi(lib.getWinApi() | scanedLib.getWinApi());
                res.unite(scanedLib._allDep);
            }
        }
    }

    libStack.remove(lib.fullPath());
}

void DependenciesScanner::addToWinAPI(const QString &lib, QHash<WinAPI, QSet<QString>>& res) {
#ifdef Q_OS_WIN
    if (QuasarAppUtils::Params::isEndable("deploySystem")) {
        WinAPI api = _peScaner.getAPIModule(lib);
        if (api != WinAPI::NoWinAPI) {
            res[api] += lib;
        }
    }
#else
    Q_UNUSED(lib)
    Q_UNUSED(res)

#endif
}

void DependenciesScanner::setEnvironment(const QStringList &env) {
    QDir dir;
    QHash<WinAPI, QSet<QString>> winAPI;

#ifdef Q_OS_WIN
    winAPI[WinAPI::Crt] += "UCRTBASE.DLL";
#endif

    for (const auto &i : env) {

        dir.setPath(i);
        if (!dir.exists()) {
            continue;
        }

        auto list = dir.entryInfoList(QStringList() << "*.dll" << "*.DLL"
                                      << "*.SO*" << "*.so*" << "*.dylib",
                                      QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);

        for (const auto &i : std::as_const(list)) {
            addToWinAPI(i.fileName().toUpper(), winAPI);
            _EnvLibs.insert(i.fileName().toUpper(), i.absoluteFilePath());
        }

        // macOS .framework bundles are directories, so the real binary
        // inside the bundle has to be registered separately under the
        // bundle file name and under the dyld framework reference
        // (Name.framework/Versions/<V>/Name) (issue #166).
        for (const auto &fw : dir.entryInfoList({"*.framework"},
                                                QDir::Dirs | QDir::NoDotAndDotDot)) {
            auto bin = DeployCore::macOSFrameworkBinary(fw.absoluteFilePath());
            if (bin.isEmpty()) {
                continue;
            }
            _EnvLibs.insert(QFileInfo(bin).fileName().toUpper(), bin);
            _EnvLibs.insert(DeployCore::normalizeMacOSLibName(bin).toUpper(), bin);
        }
    }


    _peScaner.setWinAPI(winAPI);
}

LibInfo DependenciesScanner::scan(const QString &path) {
    LibInfo info;

    if (!fillLibInfo(info, path)) {
        return info;
    }

    scan(info);
    return info;
}

void DependenciesScanner::scan(LibInfo &lib) {
    QSet<QString> stack;
    recursiveDep(lib, lib._allDep , stack);
}

DependenciesScanner::~DependenciesScanner() {

}
