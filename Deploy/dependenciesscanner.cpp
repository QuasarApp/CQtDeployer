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

QMultiMap<libPriority, LibInfo> DependenciesScanner::getLibsFromEnvirement(
        const QString &libName) {

    auto values = _EnvLibs.values(libName);
    QMultiMap<libPriority, LibInfo> res;

    for (auto & lib : values) {
        LibInfo info;

        if (!fillLibInfo(info, lib)) {
            QuasarAppUtils::Params::verboseLog(
                        "error extract lib info from " + lib + "(" + libName + ")");
            continue;
        }

        info.priority = DeployUtils::getLibPriority(info.fullPath());

        res.insertMulti(info.priority, info);
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

            auto newPriority = DeployUtils::getLibPriority(i.absoluteFilePath());
            auto oldPriority = DeployUtils::getLibPriority(_EnvLibs.value(i.fileName(), ""));

            if (newPriority > oldPriority)
                _EnvLibs.insertMulti(i.fileName(), i.absoluteFilePath());
        }

    }

}

QStringList DependenciesScanner::scan(const QString &path) {
    QStringList result;

    LibInfo info;

    if (!fillLibInfo(info, path)) {
        return result;
    }

    for (auto i : info.dependncies) {

        auto libs = getLibsFromEnvirement(i);
        result.push_back(libs.first().fullPath());
    }

    return result;
}

DependenciesScanner::~DependenciesScanner() {

}
