#include "pe.h"

#include <QFile>
#include <QFileInfo>

int PE::findIndexPE(QFile &file) {

    if (!file.isOpen()) {
        return -1;
    }

    int limit = 0xFF;
    int currentSeack = INDEX_PE_MAGIC;
    unsigned int PE = 0x0;

    while (currentSeack <= limit) {
        if (!file.seek(currentSeack)) {
            return -1;
        }

        file.read(reinterpret_cast<char*>(&PE), sizeof (PE));

        if (PE == PE_MAGIC) {
            return currentSeack;
        }

        currentSeack++;
    }

    return -1;
}

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

    int peAddress = findIndexPE(f);

    if (peAddress < 0) {
        return false;
    }

    unsigned short mashine = 0x0;
    SEEK(static_cast<unsigned int>(peAddress) + sizeof (unsigned short));

    f.read(reinterpret_cast<char*>(&mashine), sizeof (mashine));

    info.mashine = mashine;

    SEEK(static_cast<unsigned int>(peAddress) + INDEX_MAGIC);

    unsigned short magic = 0x0;
    f.read(reinterpret_cast<char*>(&magic), sizeof (magic));

    info.type = magic;

    unsigned int importTableIndex = 0;
    if (static_cast<RunType>(info.type) == RunType::_32bit) {
        importTableIndex = static_cast<unsigned int>(peAddress) + INDEX_IMPORTS_32;
    } else if (static_cast<RunType>(info.type) == RunType::_64bit) {
        importTableIndex = static_cast<unsigned int>(peAddress) + INDEX_IMPORTS_64;
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

bool PE::is32bit(const QString &file, const LIB_META_INFO * info) {

    if (!info) {
        LIB_META_INFO meta;

        if (!fillMetaInfo(meta, file)) {
            return false;
        }

        return static_cast<RunType>(meta.type) == RunType::_32bit;
    }

    return static_cast<RunType>(info->type) == RunType::_32bit;
}

bool PE::dependecies(QStringList &list, const QString &file,
                     const LIB_META_INFO * info) {


    LIB_META_INFO meta;

    if (!info) {

        if (!fillMetaInfo(meta, file)) {
            return false;
        }
    } else {
        meta = std::move(*info);
    }

    QFile f(file);

    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    if (!f.seek(meta.addressImports)) {
        f.close();
        return false;
    }

    auto data = f.read(meta.sizeImportTable).split(char(0x0));

    f.close();

    if (data.isEmpty()) {
        return false;
    }

    for (QString i : data) {
        if (i.contains(".dll")) {
            list.push_back(i);
        }
    }

    return true;
}

PE::PE(): IGetLibInfo () {

}

bool PE::getLibInfo(const QString &lib, LibInfo &info) {
    LIB_META_INFO meta;

    if (!fillMetaInfo(meta, lib)) {
        return false;
    }

    info.name = QFileInfo(lib).fileName();
    info.path = QFileInfo(lib).absolutePath();

    if (static_cast<RunType>(meta.type) == RunType::_32bit) {
        info.platform = Platform::Win32;
    } else if (static_cast<RunType>(meta.type) == RunType::_64bit) {
        info.platform = Platform::Win64;
    } else {
        info.platform = Platform::UnknownPlatform;
    }

    dependecies(info.dependncies, lib, &meta);

    return info.isValid();
}

PE::~PE(){

}
