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
    virtual bool isEmpty(const QString& package) const;

};

#endif // PACKAGECONTROL_H
