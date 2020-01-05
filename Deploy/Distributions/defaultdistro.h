#ifndef DEFAULTDISTRO_H
#define DEFAULTDISTRO_H

#include "idistribution.h"

class DEPLOYSHARED_EXPORT DefaultDistro : public iDistribution
{
public:
    DefaultDistro();

    // iDistribution interface
public:
    QString getConfig() const;
    void deployTemplate() const;
    Envirement toolKitLocation() const;
    QString runCmd() const;
    QStringList runArg() const;

};

#endif // DEFAULTDISTRO_H
