/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef MODULESQT513_H
#define MODULESQT513_H
#include "testutils.h"
#include <configparser.h>
#include <QSet>


class ModulesQt513
{
public:
    ModulesQt513();

    static QSet<QString> ignoreFilter(const QSet<QString>& input, const QString& filter);
    static QSet<QString> qtLibs();
    static QSet<QString> qmlLibs();
    static QSet<QString> qmlLibsExtractPlugins();
    static QSet<QString> separetedPackageslibs();

    static QSet<QString> outTestLibs();

    static QSet<QString> qtWithoutTr();
    static QSet<QString> qtWebEngine();
    static QSet<QString> testEmptyParamsTree(const QString &distDir = DISTRO_DIR);

};

#endif // MODULESQT513_H
