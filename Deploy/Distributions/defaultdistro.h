#ifndef DEFAULTDISTRO_H
#define DEFAULTDISTRO_H

#include "idistribution.h"

class DEPLOYSHARED_EXPORT DefaultDistro : public iDistribution
{
public:
    DefaultDistro(FileManager *);

    QString getConfig() const ;
    bool deployTemplate(PackageControl &pkg) override;
    Envirement toolKitEnv() const override;
    QList<SystemCommandData> runCmd() override;
    bool removeTemplate() const override;
    QProcessEnvironment processEnvirement() const override;

    QStringList outPutFiles() const override;

    // iDistribution interface
protected:
    QString dataLocation(const DistroModule &module) const override;
    QString location(const DistroModule &module) const override;
    QString releativeLocation(const DistroModule &module) const override;

};

#endif // DEFAULTDISTRO_H
