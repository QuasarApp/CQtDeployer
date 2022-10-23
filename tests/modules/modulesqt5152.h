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
