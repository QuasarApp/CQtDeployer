//#
//# Copyright (C) 2018-2026 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef ELF_H
#define ELF_H
#include "igetlibinfo.h"

class ELF : public IGetLibInfo
{

public:
    ELF();

    bool getLibInfo(const QString &lib, LibInfo &info) const override;

};

#endif // ELF_H
