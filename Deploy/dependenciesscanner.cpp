/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "dependenciesscanner.h"
#include "deployutils.h"

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

bool DependenciesScanner::fillLibInfo(LibInfo &info, const QString &file) {

   auto scaner = getScaner(file);

   switch (scaner) {
   case PrivateScaner::PE: {
       return _peScaner.getLibInfo(file, info);
   }

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

        auto list = dir.entryInfoList(QStringList() << "*.dll",
                                  QDir::Files| QDir::NoDotAndDotDot);

        for (auto i : list) {

            auto newPriority = DeployUtils::getLibPriority(i.absoluteFilePath());
            auto oldPriority = DeployUtils::getLibPriority(_EnvLibs.value(i.fileName(), ""));

            if (newPriority > oldPriority)
                _EnvLibs.insert(i.fileName(), i.absoluteFilePath());
        }

    }

}

QStringList DependenciesScanner::scan(const QString &path) {
    QStringList result;

    QString errorMessage;

    QStringList dep;

    if (!errorMessage.isEmpty()) {
        qCritical() << errorMessage;
        return result;
    }

    for (auto i : dep) {
        QString lib(i);
        if (_EnvLibs.count(lib)) {
            result.push_back(_EnvLibs.value(lib));
        }
    }

    return result;
}

DependenciesScanner::~DependenciesScanner() {

}
