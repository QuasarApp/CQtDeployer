#ifndef MODULESQT6_4_H
#define MODULESQT6_4_H

#include "modulesqt6_3.h"

class ModulesQt6_4: public ModulesQt63
{
public:
    ModulesQt6_4();
    QSet<QString> qmlLibs(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qmlVirtualKeyBoadrLibs(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qtWebEngine(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> qtWebEngineWidgets(const QString &distDir = DISTRO_DIR) const override;


};

#endif // MODULESQT6_4_H
