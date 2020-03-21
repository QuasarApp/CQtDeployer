#ifndef DEFAULTDISTRO_H
#define DEFAULTDISTRO_H

#include "idistribution.h"

class DEPLOYSHARED_EXPORT DefaultDistro : public iDistribution
{
public:
    DefaultDistro(FileManager *);

    // iDistribution interface
public:
    QString getConfig() const ;
    bool deployTemplate() override;
    Envirement toolKitEnv() const override;
    QString runCmd() override;
    QStringList runArg() const override;
    bool removeTemplate() const override;
    QProcessEnvironment processEnvirement() const override;

    QStringList outPutFiles() const override;
};

#endif // DEFAULTDISTRO_H
