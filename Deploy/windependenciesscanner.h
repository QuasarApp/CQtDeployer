/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef WINDEPENDENCIESSCANNER_H
#define WINDEPENDENCIESSCANNER_H

#include <QMultiMap>
#include <QStringList>
#include "deploy_global.h"
#include "structs.h"


class DEPLOYSHARED_EXPORT WinDependenciesScanner {
private:
    QStringList _env;
    QMap<QString, QString> _EnvLibs;

    bool fillLibInfo(LibInfo& info ,const QString& file);
public:
    explicit WinDependenciesScanner();

    void setEnvironment(const QStringList &env);

    QStringList scan(const QString& path,
                     Platform platfr = UnknownPlatform,
                     const QString &qmake = "qmake");

    ~WinDependenciesScanner();
};

#endif // WINDEPENDENCIESSCANNER_H
