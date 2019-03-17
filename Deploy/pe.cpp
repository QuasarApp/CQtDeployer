#include "pe.h"

#include <QFile>



bool PE::fillMetaInfo(LIB_META_INFO &info, const QString &file) {
    QFile f(file);

#define SEEK(address) \
    if (!f.seek(address)) { \
        f.close(); \
        return false; \
    }

    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    SEEK(INDEX_PE_MAGIC);

    unsigned int PE = 0x0;
    f.read(reinterpret_cast<char*>(&PE), sizeof (PE));

    if (PE != PE_MAGIC) {
        f.close();
        return false;
    }

    unsigned short mashine = 0x0;
    SEEK(INDEX_PE_MAGIC + sizeof (PE));

    f.read(reinterpret_cast<char*>(&mashine), sizeof (mashine));

    info.mashine = mashine;

    SEEK(INDEX_MAGIC);

    unsigned short magic = 0x0;
    f.read(reinterpret_cast<char*>(&magic), sizeof (magic));

    info.type = magic;

    unsigned int importTableIndex = 0;
    if (static_cast<RunType>(info.type) == RunType::_32bit) {
        importTableIndex = INDEX_IMPORTS_32;
    } else if (static_cast<RunType>(info.type) == RunType::_64bit) {
        importTableIndex = INDEX_IMPORTS_64;
    } else {
        f.close();
        return false;
    }

    SEEK(importTableIndex);

    unsigned int impoerAddress = 0x0;

    f.read(reinterpret_cast<char*>(&impoerAddress), sizeof (impoerAddress));

    SEEK(importTableIndex + sizeof (impoerAddress));

    unsigned int impoerSize = 0x0;
    f.read(reinterpret_cast<char*>(&impoerSize), sizeof (impoerSize));

    info.addressImports = impoerAddress;
    info.sizeImportTable = impoerSize;

    f.close();
    return true;
}

//TODO is sucks rewrite!
bool PE::is32bit(const QString &file) {

    LIB_META_INFO meta;

    if (!fillMetaInfo(meta, file)) {
        return false;
    }

    return static_cast<RunType>(meta.type) == RunType::_32bit;
}

bool PE::dependecies(QStringList &list, const QString &file) {
    // TODO
    LIB_META_INFO meta;

    if (!fillMetaInfo(meta, file)) {
        return false;
    }

    QFile f(file);

    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    f.close();

    if (!f.seek(meta.addressImports)) {
        f.close();
        return false;
    }

    QByteArray data = f.read(meta.sizeImportTable);

    f.close();

    if (data.isEmpty()) {
        return false;
    }

    // TODO


    return true;
}

PE::PE()
{

}
