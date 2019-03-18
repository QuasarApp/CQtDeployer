#ifndef IGETLIBINFO_H
#define IGETLIBINFO_H

#include "structs.h"

class IGetLibInfo
{
public:
    IGetLibInfo();
    virtual LibInfo &&getLibInfo(const QString& lib);
    virtual ~IGetLibInfo();

};

#endif // IGETLIBINFO_H
