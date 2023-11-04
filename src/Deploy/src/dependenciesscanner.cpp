/*
 * Copyright (C) 2018-2023 QuasarApp.
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
        return PrivateScaner::ELF;
    }

    return PrivateScaner::UNKNOWN;
}

QMultiMap<LibPriority, LibInfo> DependenciesScanner::getLibsFromEnvirement(
        const QString &libName) const {

    auto values = _EnvLibs.values(libName.toUpper());
    QMultiMap<LibPriority, LibInfo> res;

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
                QuasarAppUtils::Params::log(
                            "Failed to extract lib info from " + lib + "(" + libName + ")",
                            QuasarAppUtils::VerboseLvl::Warning);
                continue;
            }
        }

        if (!DeployCore::_config->ignoreList.isIgnore(info)) {
            res.insert(info.getPriority(), info);
        }

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

    default:
        return _filesScaner.getLibInfo(file, info);
    }
}

void DependenciesScanner::recursiveDep(LibInfo &lib, QSet<LibInfo> &res, QSet<QString>& libStack) {
    QuasarAppUtils::Params::log("Get the recursive dependencies of " + lib.fullPath(),
                                       QuasarAppUtils::Debug);

    if (_scanedLibs.contains(lib.fullPath())) {
        auto scanedLib = _scanedLibs.value(lib.fullPath());

        if (!scanedLib.isValid()) {
            QuasarAppUtils::Params::log( "Detected an invalid library in scanned library cache!!",
                                               QuasarAppUtils::Error);
            return;
        }

        res.unite(scanedLib._allDep);

        return;
    }

    if (libStack.contains(lib.fullPath())) {
        QuasarAppUtils::Params::log("A recursive dependency was found in library " + lib.fullPath(),
                                    QuasarAppUtils::Warning);
        return;
    }

    libStack.insert(lib.fullPath());

    for (const auto &i : std::as_const(lib._dependncies)) {

        auto libs = getLibsFromEnvirement(i);

        if (!libs.size()) {
            QuasarAppUtils::Params::log("Cannot find the library for dependency " + i,
                                        QuasarAppUtils::Debug);
            continue;
        }

        auto dep = libs.begin();

        while (dep != libs.end() &&
               dep.value()._platform != lib._platform) dep++;

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
                                      << "*.SO*" << "*.so*",
                                      QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);

        for (const auto &i : list) {
            addToWinAPI(i.fileName().toUpper(), winAPI);
            _EnvLibs.insert(i.fileName().toUpper(), i.absoluteFilePath());
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
