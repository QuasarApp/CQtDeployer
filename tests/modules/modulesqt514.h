/*
 * Copyright (C) 2018-2023 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef MODULESQT514_H
#define MODULESQT514_H
#include "modulesqt513.h"
#include "testutils.h"

class ModulesQt514 : public ModulesQt513
{
public:
    ModulesQt514();

    QSet<QString> qtLibs(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> onlyC(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qmlLibs(const QString &distDir = DISTRO_DIR) const override;

    QSet<QString> qtWithoutTr(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qtWebEngine(const QString &distDir = DISTRO_DIR) const override;

    QSet<QString> qtWebEngineWidgets(const QString &distDir = DISTRO_DIR) const override;
};

#endif // MODULESQT514_H
