/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef MODULES_H
#define MODULES_H

#include "testutils.h"
#include <configparser.h>
#include <QSet>



class Modules
{
private:
public:
    Modules();

    static QSet<QString> ignoreFilter(const QSet<QString>& input, const QString& filter);
    static QSet<QString> qtLibs();
    static QSet<QString> qmlLibs();
    static QSet<QString> qmlLibsExtractPlugins();

    static QSet<QString> outTestLibs();

    static QSet<QString> qtWithoutTr();
    static QSet<QString> qtWebEngine();
    static QSet<QString> testEmptyParamsTree(const QString &distDir = DISTRO_DIR);

};

#endif // MODULES_H
