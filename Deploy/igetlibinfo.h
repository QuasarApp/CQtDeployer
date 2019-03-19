#ifndef IGETLIBINFO_H
#define IGETLIBINFO_H

#include "structs.h"

class IGetLibInfo
{
public:
    IGetLibInfo() = default;
    virtual bool getLibInfo(const QString& lib, LibInfo& info) = 0;
    virtual ~IGetLibInfo() = default;

};

#endif // IGETLIBINFO_H
