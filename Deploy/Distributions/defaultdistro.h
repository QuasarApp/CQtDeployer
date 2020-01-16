#ifndef DEFAULTDISTRO_H
#define DEFAULTDISTRO_H

#include "idistribution.h"

class DEPLOYSHARED_EXPORT DefaultDistro : public iDistribution
{
public:
    DefaultDistro(FileManager *);

    // iDistribution interface
public:
    QString getConfig() const;
    bool deployTemplate();
    Envirement toolKitEnv() const;
    QString runCmd();
    QStringList runArg() const;
    bool removeTemplate() const;
    QProcessEnvironment processEnvirement() const;
};

#endif // DEFAULTDISTRO_H
