//#
//# Copyright (C) 2018-2020 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef TARGETINFO_H
#define TARGETINFO_H

#include "distrostruct.h"
#include "libinfo.h"

class DEPLOYSHARED_EXPORT TargetInfo: public LibInfo
{
public:
    TargetInfo();

    QString getSufix() const;
    void setSufix(const QString &value);

    DistroStruct& customStruct();
    const DistroStruct& getCustomStruct() const;

    void setCustomStruct(const DistroStruct &customStruct);

private:
    QString sufix = "";
    DistroStruct _customStruct;
};

#endif // TARGETINFO_H
