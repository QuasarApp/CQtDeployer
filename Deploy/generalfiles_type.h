//#
//# Copyright (C) 2021-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef GENERALFILES_TYPE_H
#define GENERALFILES_TYPE_H

#include "igetlibinfo.h"

/**
 * @brief The GeneralFiles class intendet for extract information of general files.
 * This is defautl extracte for unknown files types.
 */
class GeneralFiles: public IGetLibInfo
{
public:
    GeneralFiles();

    // IGetLibInfo interface
    bool getLibInfo(const QString &lib, LibInfo &info) const override;
};

#endif // GENERALFILES_TYPE_H
