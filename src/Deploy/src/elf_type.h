//#
//# Copyright (C) 2018-2023 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef ELF_H
#define ELF_H
#include "elfreader.h"

#include "igetlibinfo.h"

class ELF : public IGetLibInfo
{

public:
    ELF();

    bool getLibInfo(const QString &lib, LibInfo &info) const override;


private:
    QByteArrayList getDynamicString(ElfReader &reader) const;

    int getVersionOfTag(const QByteArray &tag, QByteArray &source) const;

    QString extractRPath(ElfReader &reader) const;
};

#endif // ELF_H
