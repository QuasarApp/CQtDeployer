#include "elf.h"

#include <QFileInfo>

ELF::ELF()
{

}

bool ELF::getLibInfo(const QString &lib, LibInfo &info) {
    ElfReader reader(lib);

    auto headers = reader.readHeaders();

    if (headers.elfclass == ElfClass::Elf_ELFCLASS32) {
        info.platform = Unix32;
    } else if (headers.elfclass == ElfClass::Elf_ELFCLASS64) {
        info.platform = Unix64;
    } else {
        info.platform = UnknownPlatform;
        return false;
    }

    info.name = QFileInfo(lib).fileName();
    info.path = QFileInfo(lib).absolutePath();

    for (auto &i : reader.dependencies()) {
        info.dependncies.push_back(i);
    }

    return true;
}
