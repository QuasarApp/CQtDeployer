#include "pe.h"

#include <QFile>
#include <QFileInfo>
#include <QVector>

bool PE::readSectionsHeaders(const LIB_META_INFO & info,
                             QVector<IMAGE_SECTION_HEADER> &sections,
                             QFile& file) {

    if (!file.isOpen()) {
        return false;
    }

    sections.clear();

    if (static_cast<RunType>(info.type) == RunType::_32bit) {
        if (!file.seek(info.PEIndex + SECTION_HEADER_32)) {
            return false;
        }

        sections.resize(info.rawSectionCount);
        file.read(reinterpret_cast<char*>(sections.data()),
                  sizeof (IMAGE_SECTION_HEADER) * info.rawSectionCount);

        return true;
    }
    else if (static_cast<RunType>(info.type) == RunType::_64bit) {

        if (!file.seek(info.PEIndex + SECTION_HEADER_64)) {
            return false;
        }

        sections.resize(info.rawSectionCount);
        file.read(reinterpret_cast<char*>(sections.data()),
                  sizeof (IMAGE_SECTION_HEADER) * info.rawSectionCount);
        return true;
    }

    return false;
}

DWORD PE::readSectionAligment(const LIB_META_INFO &info, QFile& file) {

    if (!file.isOpen()) {
        return false;
    }

    if (!file.seek(info.PEIndex + SECTION_ALIGMENT_INDEX_32_64)) {
        return false;
    }

    DWORD sectionAlign;

    file.read(reinterpret_cast<char*>(&sectionAlign), sizeof (sectionAlign));

    return sectionAlign;
}

unsigned short PE::findIndexPE(QFile &file) {

    if (!file.isOpen()) {
        return 0;
    }

    unsigned short limit = 0x400;
    unsigned short currentSeeck = INDEX_PE_MAGIC;
    unsigned short PE = 0x0;

    while (currentSeeck <= limit) {
        if (!file.seek(currentSeeck)) {
            return 0;
        }

        file.read(reinterpret_cast<char*>(&PE), sizeof (PE));

        if (PE == PE_MAGIC) {
            return currentSeeck;
        }

        currentSeeck++;
    }

    return 0;
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

    info.PEIndex = findIndexPE(f);

    if (!info.PEIndex) {
        return false;
    }

    SEEK(info.PEIndex + sizeof (unsigned int));
    f.read(reinterpret_cast<char*>(&info.mashine), sizeof (info.mashine));

    SEEK(info.PEIndex +
         sizeof (unsigned int) +
         sizeof (info.mashine));
    f.read(reinterpret_cast<char*>(&info.rawSectionCount),
           sizeof (info.rawSectionCount));

    SEEK(info.PEIndex + INDEX_MAGIC);

    unsigned short magic = 0x0;
    f.read(reinterpret_cast<char*>(&magic), sizeof (magic));

    info.type = magic;

    unsigned int importTableIndex = 0;

    if (static_cast<RunType>(info.type) == RunType::_32bit) {
        importTableIndex = info.PEIndex + INDEX_IMPORTS_32;

    } else if (static_cast<RunType>(info.type) == RunType::_64bit) {
        importTableIndex = info.PEIndex + INDEX_IMPORTS_64;
    } else {
        f.close();
        return false;
    }

    SEEK(importTableIndex);

    IMAGE_DATA_DIRECTORY import = {};
    f.read(reinterpret_cast<char*>(&import), sizeof (import));

    QVector<IMAGE_SECTION_HEADER> sectionHeader;
    if (!readSectionsHeaders(info, sectionHeader, f)) {
        return false;
    }

    ROW_CONVERTER converter(sectionHeader, readSectionAligment(info, f));


    info.addressImports = converter.convert(import.VirtualAddress);
    info.sizeImportTable = import.Size;

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

int ROW_CONVERTER::defSection(DWORD rva) {
    for (int i = 0; i < sections.size(); ++i) {
        DWORD start = sections[i].VirtualAddress;
        DWORD end = start + ALIGN_UP(sections[i].VirtualSize, sectionAligment);
        if(rva >= start && rva < end)
            return i;
    }
    return -1;
}

DWORD ROW_CONVERTER::rvaToOff(DWORD rva) {
    int indexSection = defSection(rva);
    if(indexSection != -1)
        return rva - sections[indexSection].VirtualAddress +
                sections[indexSection].PointerToRawData;
    else
        return 0;
}

ROW_CONVERTER::ROW_CONVERTER(QVector<IMAGE_SECTION_HEADER> sctions, DWORD align) {
    sections = sctions;
    sectionAligment = align;
}

DWORD ROW_CONVERTER::convert(DWORD rva) {
    return rvaToOff(rva);
}
