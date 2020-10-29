#ifndef DEB_H
#define DEB_H

#include "idistribution.h"

/**
 * @brief The deb class contains methods for create a debian pacakge.
 */
class DEPLOYSHARED_EXPORT Deb: public iDistribution
{
public:
    Deb(FileManager *fileManager);

    // iDistribution interface
public:
    bool deployTemplate(PackageControl &pkgCtrl) override;
    bool removeTemplate() const override;
    Envirement toolKitEnv() const override;
    QProcessEnvironment processEnvirement() const override;
    QString runCmd() override;
    QStringList runArg() const override;
    QStringList outPutFiles() const override;
};

#endif // DEB_H
