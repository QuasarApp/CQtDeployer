/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "dependenciesscanner.h"
#include "deployutils.h"
#include "quasarapp.h"
#include <QList>
#include <QDir>
#include <QDebug>

DependenciesScanner::DependenciesScanner() {}

void DependenciesScanner::clearScaned() {
    _scanedLibs.clear();
}

PrivateScaner DependenciesScanner::getScaner(const QString &lib) const {

    QFileInfo info(lib);

    auto sufix = info.completeSuffix();

    if (sufix.contains("dll", Qt::CaseSensitive) ||
            sufix.contains("exe", Qt::CaseSensitive)) {
        return PrivateScaner::PE;
    } else if (sufix.isEmpty() || sufix.contains("so", Qt::CaseSensitive)) {
        return PrivateScaner::ELF;
    }

    return PrivateScaner::UNKNOWN;
}

QMultiMap<LibPriority, LibInfo> DependenciesScanner::getLibsFromEnvirement(
        const QString &libName) {

    auto values = _EnvLibs.values(libName.toUpper());
    QMultiMap<LibPriority, LibInfo> res;

    for (auto & lib : values) {
        LibInfo info;

        if (!fillLibInfo(info, lib)) {
            QuasarAppUtils::Params::verboseLog(
                        "error extract lib info from " + lib + "(" + libName + ")",
                        QuasarAppUtils::VerboseLvl::Warning);
            continue;
        }

        info.setPriority(DeployUtils::getLibPriority(info.fullPath()));

        res.insertMulti(info.getPriority(), info);
    }

    return res;
}

bool DependenciesScanner::fillLibInfo(LibInfo &info, const QString &file) {

    info.clear();
    auto scaner = getScaner(file);

    switch (scaner) {
    case PrivateScaner::PE: {
        return _peScaner.getLibInfo(file, info);
    }
    case PrivateScaner::ELF:
        return _elfScaner.getLibInfo(file, info);

    default: return false;
    }
}

void DependenciesScanner::recursiveDep(LibInfo &lib, QSet<LibInfo> &res)
{
    QuasarAppUtils::Params::verboseLog("get recursive dependencies of " + lib.fullPath(),
                                       QuasarAppUtils::Info);

    if (_scanedLibs.contains(lib.fullPath())) {
        auto scanedLib = _scanedLibs.value(lib.fullPath());

        if (!scanedLib.isValid()) {
            qCritical() << "no valid lib in scaned libs list!";
            return;
        }

        res.unite(scanedLib.allDep);

        return;
    }

    for (auto i : lib.dependncies) {

        auto libs = getLibsFromEnvirement(i);

        if (!libs.size()) {
            QuasarAppUtils::Params::verboseLog("lib for dependese " + i + " not findet!!",
                                               QuasarAppUtils::Warning);
            continue;
        }

        auto dep = libs.begin();

        while (dep != libs.end() &&
               dep.value().platform != lib.platform) dep++;

        if (dep != libs.end() && !res.contains(*dep)) {
            res.insert(*dep);

            LibInfo scanedLib = _scanedLibs.value(dep->fullPath());

            if (!scanedLib.isValid()) {
                auto listDep =  res;

                recursiveDep(*dep, listDep);

                dep->allDep = listDep;
                _scanedLibs.insert(dep->fullPath(), *dep);

                res.unite(listDep);
            } else {
                res.unite(scanedLib.allDep);
            }
        }
    }
}

void DependenciesScanner::setEnvironment(const QStringList &env) {
    QDir dir;
    for (auto i : env) {

        dir.setPath(i);
        if (!dir.exists()) {
            continue;
        }

        auto list = dir.entryInfoList(QStringList() << "*.dll" << ".DLL"
                                      << "*.SO*" << "*.so*",
                                      QDir::Files| QDir::NoDotAndDotDot);

        for (auto i : list) {

            _EnvLibs.insertMulti(i.fileName().toUpper(), i.absoluteFilePath());
        }

    }

}

QSet<LibInfo> DependenciesScanner::scan(const QString &path) {
    QSet<LibInfo> result;

    LibInfo info;

    if (!fillLibInfo(info, path)) {
        return result;
    }

    recursiveDep(info, result);

    return result;
}

DependenciesScanner::~DependenciesScanner() {

}
