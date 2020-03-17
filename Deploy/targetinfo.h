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

    QString getPackage() const;
    void setPackage(const QString &value);

private:
    QString package = "";
};

#endif // TARGETINFO_H
