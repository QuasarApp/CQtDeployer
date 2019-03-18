/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "windependenciesscanner.h"
#include "deployutils.h"

#include <QList>
#include <QDir>
#include <QDebug>

WinDependenciesScanner::WinDependenciesScanner() {}


bool WinDependenciesScanner::fillLibInfo(LibInfo &info, const QString &file) {

}

void WinDependenciesScanner::setEnvironment(const QStringList &env) {
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

QStringList WinDependenciesScanner::scan(const QString &path, Platform platfr,
                                         const QString& qmake) {
    QStringList result;

    QString errorMessage;

    if (platfr == Platform::UnknownPlatform) {

    }

    QStringList dep;
//    readExecutable(path, platfr, &errorMessage, &dep);

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

WinDependenciesScanner::~WinDependenciesScanner() {

}
