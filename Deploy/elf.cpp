#include "elf.h"

#include <QFileInfo>

ELF::ELF()
{

}

bool ELF::getLibInfo(const QString &lib, LibInfo &info) {
    ElfReader reader(lib);

    auto headers = reader.readHeaders();

    if (headers.elfclass == ElfClass::Elf_ELFCLASS32) {
        info.setPlatform(Unix32);
    } else if (headers.elfclass == ElfClass::Elf_ELFCLASS64) {
        info.setPlatform(Unix64);
    } else {
        info.setPlatform(UnknownPlatform);
        return false;
    }

    info.setName(QFileInfo(lib).fileName());
    info.setPath(QFileInfo(lib).absolutePath());

    for (auto &i : reader.dependencies()) {
        info.addDependncies(i);
    }

    return true;
}
