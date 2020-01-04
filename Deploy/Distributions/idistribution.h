#ifndef IDISTRIBUTION_H
#define IDISTRIBUTION_H

#include "distrostruct.h"
#include "envirement.h"

class DEPLOYSHARED_EXPORT iDistribution
{
public:
    iDistribution();
    virtual ~iDistribution();

    virtual QString getConfig() = 0;
    virtual Envirement toolKitLocation() = 0;
    virtual QString runCmd() = 0;
};

#endif // IDISTRIBUTION_H
