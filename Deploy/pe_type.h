//#
//# Copyright (C) 2018-2021 QuasarApp.
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
    QHash<WinAPI, QSet<QString>> _winAPI;

public:

    PE();
    WinAPI getAPIModule(const QString &libName) const;

    bool getLibInfo(const QString& lib, LibInfo& info) const override;

    ~PE() override;

    QHash<WinAPI, QSet<QString>> getWinAPI() const;
    void setWinAPI(const  QHash<WinAPI, QSet<QString>> &winAPI);
};

#endif // PE_H
