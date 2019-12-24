//#
//# Copyright (C) 2018-2019 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef PE_H
#define PE_H

#include <QFile>
#include <QString>
#include <QVector>
#include "igetlibinfo.h"

#define API_MS_WIN "api-ms-win-"
#define API_MS_WIN_CORE "-core-"
#define API_MS_WIN_EVENTING "-Eventing-"
#define API_MS_WIN_DEVICES "-devices-"
#define API_MS_WIN_CRT "-crt-"
#define API_MS_WIN_SECURITY "-security-"
#define API_MS_WIN_BASE "-base-"

namespace peparse {
struct parsed_pe_internal;
}
class PE : public IGetLibInfo {

private:

    bool getDep(peparse::parsed_pe_internal *, LibInfo &res) const;

public:

    enum class RunType: unsigned short {
        _UNKNOWN = 0x0,
        _32bit = 0x10B,
        _64bit = 0x20B,
        _ROM = 0x107,
    };
    PE();
    WinAPI getAPIModule(const QString &libName) const;

    bool getLibInfo(const QString& lib, LibInfo& info) const override;

    ~PE() override;

};

#endif // PE_H
