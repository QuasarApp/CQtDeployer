/*
 * Copyright (C) 2018-2024 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef DEPLOYCONFIG_H
#define DEPLOYCONFIG_H
#include "deploy_global.h"

#include "distromodule.h"
#include "extra.h"
#include "ignorerule.h"
#include "qtdir.h"
#include "targetinfo.h"

class DEPLOYSHARED_EXPORT DeployConfig {

public:
    /**
     * @brief depchLimit - recursive search limit
     */
    int depchLimit = 0;

    /**
     * @brief deployQml - enable or disable deploing of qml files.
     */
    bool deployQml = false;

    /**
     * @brief ignoreList - list with ignore files
     */
    IgnoreRule ignoreList;

    /**
     * @brief appDir - it is cqtdeployer library location for ignre cqtdeployr libraries
     */
    QString appDir;

    /**
     * @brief qtDir - conteins all qt pathes
     */
    QtDir qtDir;

    /**
     * @brief extraPaths - it is list with filters for extra pathes, files or libraries
     */
    Extra extraPaths;

    /**
     * @brief allowedPaths  - it is list with filters for allowed pathes, files or libraries
     */
    Extra allowedPaths;

    /**
     * @brief envirement - envirement for find libraries
     */
    Envirement envirement;

    /**
     * @brief reset config file to default
     */
    void reset();
    QHash<QString, TargetInfo *> getTargetsListByFilter(const QString& filter);
    QHash<QString, const TargetInfo *> getTargetsListByFilter(const QString& filter) const;

    /**
     * @brief targetPath
     * @param target
     * @return path to deployement root dir of target.
     * If target value well be empty then return main root of all targets.
     */
    QString getTargetDir(const QString& target = "") const;

    /**
     * @brief getPackageTargetDir This method return the target dif of the package.
     * @param package This is id of the package
     * @return target diractory path
     */
    QString getPackageTargetDir(const QString& package) const;

    /**
     * @brief setTargetDir
     * @param target
     */
    void setTargetDir(const QString& target);

    /**
     * @brief getDistro
     * @param target
     * @return
     */
    DistroModule getDistro(const QString& target) const;

    /**
     * @brief getDistroFromPackage
     * @param package
     * @return distro struct from package
     */
    DistroModule getDistroFromPackage(const QString& package) const;

    const QHash<QString, TargetInfo>& targets() const;
    const QHash<QString, DistroModule>& packages() const;

    QHash<QString, TargetInfo> &targetsEdit();
    QHash<QString, DistroModule>& packagesEdit();

    /**
     * @brief getPlatform This method return a platform of distribution.
     *  If you set a package name then this method return platform of package.
     * @param package This is name of package. Set this parameter to empty string for get a distribution of all packages.
     * @return platform of distribution.
     */
    Platform getPlatform(const QString& package) const;

    /**
     * @brief getPlatformOfAll This method return paltform of all targets of distributuon.
     * @return platform of all targets.
     */
    Platform getPlatformOfAll() const;

    QString getDefaultPackage() const;
    void setDefaultPackage(const QString &value);

     /**
     * @brief isNeededQt This method return all needed qt major version for all targets.
     * @return qt major version
     */
    QtMajorVersion isNeededQt() const;

    /**
     * @brief isNeededQt This method return all needed qt major versions for the @a package.
     * @param package This is validation package.
     * @return qt major version
     */
    QtMajorVersion isNeededQt(const QString& package) const;

    /**
     * @brief customPlatform This is custom platform of distribution
     * @return custom platform
     */
    Platform customPlatform() const;

    /**
     * @brief setCustomPlatform This method sets custom platform for this distribution.
     * @param newCustomPlatform new custom platform.
     */
    void setCustomPlatform(Platform newCustomPlatform);

private:

    /**
     * @brief targets
     * key - path
     * value - create wrapper
     */
    QHash<QString, TargetInfo> _targets;

    /**
     * @brief packages
     * key - package
     * value struvture of package
     * default package is empty value
     */
    QHash<QString, DistroModule> _packages;

    /**
     * @brief targetDir -  targe directory (this folder conteins all files of distrebution kit)
     */
    QString targetDir = "";
    QString defaultPackage = "";

    Platform _customPlatform = Platform::UnknownPlatform;


};


#endif // DEPLOYCONFIG_H
