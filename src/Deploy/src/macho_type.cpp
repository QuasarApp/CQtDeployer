//#
//# Copyright (C) 2018-2026 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "macho_type.h"
#include "deploycore.h"
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

    // A fat (universal) binary contains one slice per architecture. dyld
    // loads the slice of the running architecture, so the scanner uses the
    // same slice: the first slice with the host CPU type (issue #166).
#if defined(__aarch64__) || defined(__arm64__)
    constexpr auto hostCpu = LIEF::MachO::Header::CPU_TYPE::ARM64;
#elif defined(__x86_64__)
    constexpr auto hostCpu = LIEF::MachO::Header::CPU_TYPE::X86_64;
#else
    constexpr auto hostCpu = LIEF::MachO::Header::CPU_TYPE::ANY;
#endif

    const LIEF::MachO::Binary *slice = nullptr;
    for (const LIEF::MachO::Binary &bin : *macho) {
        if (bin.header().cpu_type() == hostCpu) {
            slice = &bin;
            break;
        }
    }

    if (slice == nullptr) {
        // No slice for the host architecture: fall back to the first slice
        // with the same bitness as the host (or the first slice at all).
        slice = macho->front();
        for (const LIEF::MachO::Binary &bin : *macho) {
            if (bin.header().is_64bit() == (sizeof(void*) == 8)) {
                slice = &bin;
                break;
            }
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

    // Extract the runtime search paths (LC_RPATH). They are used to find
    // the qmake of a Qt installation (issue #166). The entries are read via
    // DeployCore::machORPaths(), which parses the binary on the LIEF backend
    // and reads the entries from the raw command data, because the LIEF
    // submodule maps LC_RPATH to a different load command constant than the
    // macOS ABI.
    info.setRPath(DeployCore::machORPaths(infolib.absoluteFilePath())
                     .join(QLatin1String(":")));

    return true;
}
