#ifndef DEPENDENCIESMAP_H
#define DEPENDENCIESMAP_H

#include "deploy_global.h"
#include "deploycore.h"
#include <QSet>

/**
 * @brief The DependenciesMap class
 * this class contains all dependencies information about target
 */
class DEPLOYSHARED_EXPORT DependenciesMap
{
public:
    DependenciesMap();

    DependenciesMap& operator +=(const DependenciesMap& other);
    DependenciesMap& operator -=(const DependenciesMap& other);

    DeployCore::QtModule qtModules() const;
    const QSet<QString>& neadedLibs() const;
    const QSet<QString>& systemLibs() const;

private:
    DeployCore::QtModule _qtModules = DeployCore::NONE;

    QSet<QString> _neadedLibs;
    QSet<QString> _systemLibs;
};

#endif // DEPENDENCIESMAP_H
