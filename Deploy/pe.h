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

#define APU_MS_WIN "api-ms-win"

namespace peparse {
struct parsed_pe_internal;
}
class PE : public IGetLibInfo {

private:

    bool getDep(peparse::parsed_pe_internal *, LibInfo &res) const;

    QSet<QString> _apimswin;

public:

    enum class RunType: unsigned short {
        _UNKNOWN = 0x0,
        _32bit = 0x10B,
        _64bit = 0x20B,
        _ROM = 0x107,
    };
    PE();

    /**
     * @brief setWinApiPlugins
     * @param keys set values of api-ms-win libs
     */
    void setWinApiPlugins(const QSet<QString>& keys);

    bool getLibInfo(const QString& lib, LibInfo& info) const override;

    ~PE() override;

};

#endif // PE_H
