#ifndef IDISTRIBUTION_H
#define IDISTRIBUTION_H

#include "distrostruct.h"
#include "envirement.h"

class DEPLOYSHARED_EXPORT iDistribution
{
public:
    iDistribution();
    virtual ~iDistribution();

    virtual DistroStruct getStruct() = 0;
    virtual Envirement toolKitLocation() = 0;
};

#endif // IDISTRIBUTION_H
