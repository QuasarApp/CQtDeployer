/*
 * Copyright (C) 2018-2021 QuasarApp.
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
    virtual ~ModulesQt513();

    virtual QSet<QString> ignoreFilter(const QSet<QString>& input, const QString& filter) const;
    virtual QSet<QString> replace(const QSet<QString> &data, const QMap<QString, QString>& replaceMap) const;


    virtual QSet<QString> onlyC(const QString &distDir = DISTRO_DIR) const;
    virtual QSet<QString> qtLibs(const QString &distDir = DISTRO_DIR) const;
    virtual QSet<QString> qmlLibs(const QString &distDir = DISTRO_DIR) const;
    virtual QSet<QString> qmlVirtualKeyBoadrLibs(const QString &distDir = DISTRO_DIR) const;

    virtual QSet<QString> testDistroLibs(const QString &distDir = DISTRO_DIR) const;
    virtual QSet<QString> testOutLibs(const QString &distDir = DISTRO_DIR) const;

    virtual QSet<QString> qtWithoutTr(const QString &distDir = DISTRO_DIR) const;
    virtual QSet<QString> qtWebEngine(const QString &distDir = DISTRO_DIR) const;
    virtual QSet<QString> qtWebEngineWidgets(const QString &distDir = DISTRO_DIR) const;
    virtual QSet<QString> testEmptyParamsTree(const QString &distDir = DISTRO_DIR) const;


};

#endif // MODULESQT513_H
