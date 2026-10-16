//#
//# Copyright (C) 2018-2026 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "macho_type.h"
#include <QFileInfo>
#include <quasarapp.h>

#include <LIEF/MachO.hpp>

MachO::MachO()
{

}

bool MachO::getLibInfo(const QString &lib, LibInfo &info) const {
    QFileInfo infolib(lib);
    if (!infolib.exists()) {
        return false;
    }

    auto macho = LIEF::MachO::Parser::parse(infolib.absoluteFilePath().toStdString());
    if (macho == nullptr || macho->empty()) {
        QuasarAppUtils::Params::log("Unable to parse Mach-O file: " + infolib.absoluteFilePath(),
                                    QuasarAppUtils::Error);
        return false;
    }

    // A Mach-O file can be a fat (universal) binary with several architecture
    // slices. Prefer the slice with the same bitness as the running host,
    // otherwise fall back to the first slice.
    const LIEF::MachO::Binary *slice = macho->front();
    for (const LIEF::MachO::Binary &bin : *macho) {
        if (bin.header().is_64bit() == (sizeof(void*) == 8)) {
            slice = &bin;
            break;
        }
    }

    switch (slice->header().cpu_type()) {
    case LIEF::MachO::Header::CPU_TYPE::X86_64:
        info.setPlatform(Mac_X86_64);
        break;

    case LIEF::MachO::Header::CPU_TYPE::ARM64:
        info.setPlatform(Mac_ARM_64);
        break;

    default:
        info.setPlatform(UnknownPlatform);
        return false;
    }

    info.setName(infolib.fileName());
    info.setPath(infolib.absolutePath());

    for (const LIEF::MachO::DylibCommand &dylib : slice->libraries()) {
        const auto &name = dylib.name();
        if (name.empty())
            continue;

        info.addDependncies(QString::fromStdString(name));
    }

    // Symbol-level information: the symbols this slice imports (bound to a
    // dylib through the two-level namespace) and the symbols it defines.
    // Mach-O has no per-symbol versions, so plain names are stored. Used by
    // the dependencies scanner to pick the right candidate when several
    // dylibs with the same name exist in the environment.
    for (const auto &symbol : slice->imported_symbols()) {
        if (!symbol.name().empty()) {
            info.addRequiredSymbol(QString::fromStdString(symbol.name()));
        }
    }

    for (const auto &symbol : slice->exported_symbols()) {
        if (!symbol.name().empty()) {
            info.addProvidedSymbol(QString::fromStdString(symbol.name()));
        }
    }

    return true;
}