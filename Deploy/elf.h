#ifndef ELF_H
#define ELF_H
#include "../qtTools/src/shared/winutils/elfreader.h"

#include "igetlibinfo.h"


class ELF : public IGetLibInfo
{

public:
    ELF();

    bool getLibInfo(const QString &lib, LibInfo &info);
};

#endif // ELF_H
