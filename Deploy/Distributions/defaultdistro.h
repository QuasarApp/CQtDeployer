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
    QString runCmd() override;
    QStringList runArg() const override;
    bool removeTemplate() const override;
    QProcessEnvironment processEnvirement() const override;

    QStringList outPutFiles() const override;

    // iDistribution interface
protected:
    QString dataLocation(const QString &packageName) const override;
    QString location(const QString &packageName) const override;
};

#endif // DEFAULTDISTRO_H
