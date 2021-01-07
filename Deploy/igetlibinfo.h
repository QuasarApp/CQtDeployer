//#
//# Copyright (C) 2018-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef IGETLIBINFO_H
#define IGETLIBINFO_H

#include "deploycore.h"
#include "libinfo.h"

class IGetLibInfo
{
public:
    IGetLibInfo() = default;
    virtual bool getLibInfo(const QString& lib, LibInfo& info) const = 0;
    virtual ~IGetLibInfo() = default;

};

#endif // IGETLIBINFO_H
