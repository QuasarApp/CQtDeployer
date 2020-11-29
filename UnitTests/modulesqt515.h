#ifndef MODULESQT515_H
#define MODULESQT515_H

#include "modulesqt514.h"

class ModulesQt515 : public ModulesQt514
{
public:
    ModulesQt515();

    // ModulesQt513 interface
public:
    QSet<QString> onlyC(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qtLibs(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qmlLibs(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qtWithoutTr(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qtWebEngine(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> testDistroLibs(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> testOutLibs(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qtWebEngineWidgets(const QString &distDir = DISTRO_DIR) const override;
};

#endif // MODULESQT515_H
