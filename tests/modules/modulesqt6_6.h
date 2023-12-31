/*
 * Copyright (C) 2023-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef MODULESQT6_6_H
#define MODULESQT6_6_H

#include "modulesqt6_5.h"

class ModulesQt6_6: public ModulesQt6_5
{
public:
    ModulesQt6_6();
    QSet<QString> qmlLibs(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qmlVirtualKeyBoadrLibs(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qtWebEngine(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qtWebEngineWidgets(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qtLibs(const QString &distDir = DISTRO_DIR) const override;
};

#endif // MODULESQT6_5_H
