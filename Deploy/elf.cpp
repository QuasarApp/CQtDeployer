//#
//# Copyright (C) 2018-2020 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "elf.h"
#include <cmath>
#include <QFileInfo>
#include <quasarapp.h>

ELF::ELF()
{

}

QByteArrayList ELF::getDynamicString(ElfReader& reader) const {
    auto headers = reader.readHeaders();

    for (auto &sectionHeader : headers.sectionHeaders) {
        if (sectionHeader.name == ".dynstr") {
            auto arr = reader.readSection(sectionHeader.name).split(0);
            return arr;
        }
    }

    return {};
}

int ELF::getVersionOfTag(const QByteArray& tag, QByteArray& source) const {
    auto versions = source.replace(tag, "").split('.');

    int step = static_cast<int>(pow(100, 4));
    int ver = 0;
    auto it = versions.begin();

    while (it != versions.end()) {
        bool ok;
        int curVer = it->toInt(&ok);

        if (!ok) {
            return -1;
        }
        ver += curVer * step;
        step /= 100;
        it++;
    }

    return ver;
}

bool ELF::getLibInfo(const QString &lib, LibInfo &info) const {
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

    if (!QuasarAppUtils::Params::isEndable("noAutoCheckQmake")) {
        auto dynStr = getDynamicString(reader);

        for (auto i = dynStr.rbegin(); i != dynStr.rend(); ++i) {

            if (i->contains("end_")) {
                break;
            }

            if (QFileInfo(*i).isDir()) {
                info.setQtPath(*i);
            }

        }
    }

    info.setName(QFileInfo(lib).fileName());
    info.setPath(QFileInfo(lib).absolutePath());

    auto dep = reader.dependencies();
    for (auto &i : dep) {
        info.addDependncies(i.toUpper());
    }

    return true;
}
