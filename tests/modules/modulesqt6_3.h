#ifndef MODULESQT6_3_H
#define MODULESQT6_3_H

#include "modulesqt5152.h"

class ModulesQt63: public ModulesQt5152
{
public:
    ModulesQt63();

    // ModulesQt6.3 interface
public:
    QSet<QString> qmlLibs(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> testDistroLibs(const QString &distDir = DISTRO_DIR) const override;
    QSet<QString> testOutLibs(const QString &distDir = DISTRO_DIR) const override;

    // ModulesQt513 interface
public:
    QSet<QString> ignoreFilter(const QSet<QString> &input, const QString &filter) const override;
    QSet<QString> replace(const QSet<QString> &data, const QMap<QString, QString> &replaceMap) const override;
    QSet<QString> onlyC(const QString &distDir) const override;
    QSet<QString> qtLibs(const QString &distDir) const override;
    QSet<QString> qmlVirtualKeyBoadrLibs(const QString &distDir) const override;
    QSet<QString> qtWithoutTr(const QString &distDir) const override;
    QSet<QString> qtWebEngine(const QString &distDir) const override;
    QSet<QString> qtWebEngineWidgets(const QString &distDir) const override;
    QSet<QString> testEmptyParamsTree(const QString &distDir) const override;
};

#endif // MODULESQT6_3_H
