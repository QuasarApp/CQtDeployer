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
    virtual bool movePackage(const QString& package,
                             const QString& newLocation) = 0;

};

#endif // PACKAGECONTROL_H
