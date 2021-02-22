//#
//# Copyright (C) 2021-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef MACHO_H
#define MACHO_H

#include "igetlibinfo.cpp"

/**
 * @brief The MachO class is wraper class for the extract dependecies of the Mach Object files.
 */
class MachO : public IGetLibInfo
{
public:
    MachO();

    // IGetLibInfo interface
    bool getLibInfo(const QString &lib, LibInfo &info) const override;
};

#endif // MACHO_H
