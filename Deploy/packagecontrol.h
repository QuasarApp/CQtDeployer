/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef PACKAGECONTROL_H
#define PACKAGECONTROL_H

#include <QString>


/**
 * @brief The PackageControl class - this is interface for move files for package
 */
class PackageControl
{
public:
    PackageControl();
    virtual ~PackageControl();

    /**
     * @brief movePackage This method should be move package to new location.
     * @param package This is key of deployed package.
     * @param newLocation This is path to new location of package.
     * @return true if data moved successful.
     */
    virtual bool movePackage(const QString& package,
                             const QString& newLocation) = 0;

    /**
     * @brief copyPackage This method should be copy package to new location.
     * @param package This is key of deployed package.
     * @param newLocation This is path to new location of package.
     * @return true if data copyed successful.
     */
    virtual bool copyPackage(const QString& package,
                             const QString& newLocation) = 0;

    /**
     * @brief isEmpty This method should be return true if the "package" is empty.
     * @param package This is key of deployed package.
     * @return true if the package is empty.
     * @note this method using for skiping empty packages on the packing step.
     */
    bool isEmpty(const QString& package) const;

    /**
     * @brief availablePackages This method return list of prepared for packaing packages.
     * @note This list maybe have a difference with the packages list from main config because some packages do not have any files.
     * @return list of keys of available packages.
     */
    virtual QStringList availablePackages() const = 0;


};

#endif // PACKAGECONTROL_H
