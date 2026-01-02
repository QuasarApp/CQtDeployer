/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef MODULESQT5152_H
#define MODULESQT5152_H

#include "modulesqt515.h"

class ModulesQt5152: public ModulesQt515
{
public:
    ModulesQt5152();

    // ModulesQt513 interface
public:
    QSet<QString> qmlLibs(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> testDistroLibs(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> testOutLibs(const QString &distDir = DISTRO_DIR) const override;
};

#endif // MODULESQT5152_H
