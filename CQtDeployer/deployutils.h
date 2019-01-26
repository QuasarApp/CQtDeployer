/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef DEPLOYUTILS_H
#define DEPLOYUTILS_H

#include <QStringList>
#include <QDebug>

class Deploy;

class DeployUtils
{
public:
    DeployUtils() = delete;

    static QString qtDir;
    static QStringList extraPaths;

    static bool isQtLib(const QString &lib);
    static bool isExtraLib(const QString &lib);
    static int getLibPriority(const QString &lib);
    static void verboseLog(const QString &str);
    static void help();
    static bool parseQt(Deploy *deploy);
};

#endif // DEPLOYUTILS_H
