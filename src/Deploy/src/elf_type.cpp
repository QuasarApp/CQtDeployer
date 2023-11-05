//#
//# Copyright (C) 2018-2023 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "elf_type.h"
#include <cmath>
#include <QFileInfo>
#include <quasarapp.h>

ELF::ELF()
{

}

QByteArrayList ELF::getDynamicString(ElfReader& reader) const {
    auto headers = reader.readHeaders();

    for (const auto &sectionHeader : std::as_const(headers.sectionHeaders)) {
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

QString ELF::extractRPath(ElfReader& reader) const {

    QString result;
    if (!QuasarAppUtils::Params::isEndable("noCheckRPATH")) {
        auto dynStr = getDynamicString(reader);

        for (auto i = dynStr.rbegin(); i != dynStr.rend(); ++i) {

            if (i->contains("end_")) {
                return result;
            }

            const auto pathes = i->split(DeployCore::getEnvSeparator());
            for (const auto &path: pathes) {
                if (path.contains("/")) {
                    if (result.size()) {
                        result += DeployCore::getEnvSeparator() + DeployCore::transportPathToSnapRoot(path);
                    } else {
                        result += DeployCore::transportPathToSnapRoot(path);
                    }
                }
            }

            if (result.size())
                return result;
        }
    }

    return result;
}

bool ELF::getLibInfo(const QString &lib, LibInfo &info) const {
    ElfReader reader(lib);

    auto headers = reader.readHeaders();

    const int Elf_EM_ARM64  = 0xb7;

    if (headers.elfmachine == ElfMachine::Elf_EM_ARM ||
           static_cast<int>(headers.elfmachine) == Elf_EM_ARM64 ) {

        if (headers.elfclass == ElfClass::Elf_ELFCLASS32) {
            info.setPlatform(Unix_ARM_32);

        } else {
            info.setPlatform(Unix_ARM_64);
        }

    } else if (headers.elfmachine == ElfMachine::Elf_EM_X86_64 ||
               headers.elfmachine == ElfMachine::Elf_EM_386) {

        if (headers.elfclass == ElfClass::Elf_ELFCLASS32) {
            info.setPlatform(Unix_x86_32);

        } else {
            info.setPlatform(Unix_x86_64);
        }

    } else {
        info.setPlatform(UnknownPlatform);
        return false;
    }

    QFileInfo infolib(lib);
    info.setName(infolib.fileName());
    info.setPath(infolib.absolutePath());
    info.setRPath(extractRPath(reader));

    auto dep = reader.dependencies();
    for (const auto &i : dep) {
        info.addDependncies(i);
    }

    return true;
}
