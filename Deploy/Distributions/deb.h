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
    QList<SystemCommandData> runCmd() override;
    QStringList outPutFiles() const override;
    bool cb() const override;

    // iDistribution interface
protected:
    QString dataLocation(const DistroModule &module) const override;
    QString location(const DistroModule &module) const override;
    QString releativeLocation(const DistroModule &module) const override;

private:
    QStringList outFiles;
    QStringList packageFolders;

};

#endif // DEB_H
