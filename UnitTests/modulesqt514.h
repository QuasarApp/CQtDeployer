/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef MODULESQT514_H
#define MODULESQT514_H
#include "modulesqt513.h"

class ModulesQt514 : public ModulesQt513
{
public:
    ModulesQt514();

    static QSet<QString> qtLibs();
    static QSet<QString> qmlLibs();
    static QSet<QString> qmlLibsExtractPlugins();
    static QSet<QString> separetedPackageslibs();

    static QSet<QString> outTestLibs();

    static QSet<QString> qtWithoutTr();
    static QSet<QString> qtWebEngine();
};

#endif // MODULESQT514_H
