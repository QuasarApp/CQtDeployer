#ifndef ELF_H
#define ELF_H
#include "../qtTools/src/shared/winutils/elfreader.h"

#include "igetlibinfo.h"

class ELF : public IGetLibInfo
{

private:
    QByteArrayList getDynamicString(ElfReader &reader) const;

    int getVersionOfTag(const QByteArray &tag, QByteArray &source) const;

public:
    ELF();

    bool getLibInfo(const QString &lib, LibInfo &info) const override;
};

#endif // ELF_H
