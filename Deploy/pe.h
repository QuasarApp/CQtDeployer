#ifndef PE_H
#define PE_H

#include <QFile>
#include <QString>
#include <QVector>
#include "igetlibinfo.h"

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

    bool getLibInfo(const QString& lib, LibInfo& info) const override;

    ~PE() override;

};

#endif // PE_H
