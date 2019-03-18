#ifndef PE_H
#define PE_H

#include <QString>
#include "igetlibinfo.h"

struct LIB_META_INFO {
    unsigned short mashine = 0x0;
    unsigned short type = 0x0;
    unsigned int addressImports = 0x0;
    unsigned int sizeImportTable = 0x0;
};

class PE : public IGetLibInfo {

private:
    bool fillMetaInfo(LIB_META_INFO& info, const QString &file);

    constexpr static unsigned int PE_MAGIC = 0x00004550;
    constexpr static unsigned int INDEX_PE_MAGIC = 0x80;
    constexpr static unsigned int INDEX_MAGIC = INDEX_PE_MAGIC + 0x16;
    constexpr static unsigned int INDEX_IMPORTS_32 = INDEX_MAGIC + 0x68;
    constexpr static unsigned int INDEX_IMPORTS_64 = INDEX_MAGIC + 0x78;

public:
    enum class MashineTypesS: unsigned short {
        IMAGE_FILE_MACHINE_UNKNOWN = 0x0,
        IMAGE_FILE_MACHINE_AMD64 = 0x8664,    // x64
        IMAGE_FILE_MACHINE_ARM = 0x1c0,       // ARM little endian
        IMAGE_FILE_MACHINE_ARM64 = 0xaa64,    // ARM64 little endian
        IMAGE_FILE_MACHINE_ARMNT = 0x1c4,     // ARM Thumb-2 little endian
        IMAGE_FILE_MACHINE_I386 = 0x14c,      // Intel 386 or later processors and compatible processors
    };

    enum class RunType: unsigned short {
        _UNKNOWN = 0x0,
        _32bit = 0x10B,
        _64bit = 0x20B,
        _ROM = 0x107,
    };

    bool is32bit(const QString& file, const LIB_META_INFO *info = nullptr);
    bool dependecies(QStringList& lisr, const QString& file,
                     const LIB_META_INFO *info = nullptr);
    PE();

    LibInfo &&getLibInfo(const QString& lib) override;

    ~PE() override;

};

#endif // PE_H
