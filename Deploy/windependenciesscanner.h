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
#include "../qtTools/src/windeployqt/utils.h"
#include "deploy_global.h"

struct LibInfo {
    Platform platform = Platform::UnknownPlatform;
    bool is32bit = false;
    QString name;
    QString path;

    bool operator == (const LibInfo& other);

    QString fullPath();
};

class DEPLOYSHARED_EXPORT WinDependenciesScanner {
private:
    QStringList _env;
    QMap<QString, QString> _EnvLibs;
    Platform platformFromMkSpec(const QString &xSpec);
    QMap<QString, QString> qMakeAll(QString *errorMessage,
                                    const QString &binary = "qmake");

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
