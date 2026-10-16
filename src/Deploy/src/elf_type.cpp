//#
//# Copyright (C) 2018-2026 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#include "elf_type.h"
#include <QFileInfo>
#include <quasarapp.h>

#include <LIEF/ELF.hpp>

namespace {

/// Extract the runtime library search path (DT_RUNPATH takes precedence over
/// DT_RPATH, like the dynamic loader). The path list is filtered to absolute
/// entries and transported to the snap root, like the rest of the deployer.
QString extractRPath(const LIEF::ELF::Binary &elf) {
    QString rpath;
    QString runpath;

    for (const auto &entry : elf.dynamic_entries()) {
        if (const auto *runPath = entry.cast<LIEF::ELF::DynamicEntryRunPath>()) {
            runpath = QString::fromStdString(runPath->runpath());
        } else if (const auto *rPath = entry.cast<LIEF::ELF::DynamicEntryRpath>()) {
            rpath = QString::fromStdString(rPath->rpath());
        }
    }

    const QString &selected = runpath.isEmpty() ? rpath : runpath;
    if (selected.isEmpty()) {
        return {};
    }

    QStringList result;
    for (const auto &path : selected.split(DeployCore::getEnvSeparator())) {
        if (path.contains("/")) {
            result.append(QuasarAppUtils::PlatformUtils::transportPathToSnapRoot(path));
        }
    }

    return result.join(QString(DeployCore::getEnvSeparator()));
}

/// Format a symbol the way the deployer stores it: plain name for
/// unversioned symbols and "name@version" for versioned ones (the ELF
/// symbol version, e.g. "GLIBCXX_3.4.31").
QString formatSymbol(const LIEF::ELF::Symbol &symbol) {
    QString name = QString::fromStdString(symbol.name());

    if (symbol.has_version() && symbol.symbol_version()->has_auxiliary_version()) {
        const auto &version = symbol.symbol_version()->symbol_version_auxiliary()->name();
        if (!version.empty()) {
            name += QStringLiteral("@") + QString::fromStdString(version);
        }
    }

    return name;
}

} // namespace

ELF::ELF()
{

}

bool ELF::getLibInfo(const QString &lib, LibInfo &info) const {
    QFileInfo infolib(lib);
    if (!infolib.exists()) {
        return false;
    }

    auto elf = LIEF::ELF::Parser::parse(infolib.absoluteFilePath().toStdString());
    if (elf == nullptr) {
        return false;
    }

    const auto machine = elf->header().machine_type();
    const bool is64 = elf->header().identity_class() == LIEF::ELF::Header::CLASS::ELF64;

    if (machine == LIEF::ELF::ARCH::ARM || machine == LIEF::ELF::ARCH::AARCH64) {

        if (is64) {
            info.setPlatform(Unix_ARM_64);

        } else {
            info.setPlatform(Unix_ARM_32);
        }

    } else if (machine == LIEF::ELF::ARCH::X86_64 ||
               machine == LIEF::ELF::ARCH::I386) {

        if (is64) {
            info.setPlatform(Unix_x86_64);

        } else {
            info.setPlatform(Unix_x86_32);
        }

    } else {
        info.setPlatform(UnknownPlatform);
        return false;
    }

    info.setName(infolib.fileName());
    info.setPath(infolib.absolutePath());

    if (!QuasarAppUtils::Params::isEndable("noCheckRPATH")) {
        info.setRPath(extractRPath(*elf));
    }

    for (const auto &entry : elf->dynamic_entries()) {
        if (const auto *needed = entry.cast<LIEF::ELF::DynamicEntryLibrary>()) {
            const auto &name = needed->name();
            if (!name.empty()) {
                info.addDependncies(QString::fromStdString(name));
            }
        }
    }

    // Symbol-level information: the undefined dynamic symbols this library
    // requires (with their symbol versions, e.g. "GLIBCXX_3.4.31"), the
    // dynamic symbols it provides and the version nodes it defines. Used by
    // the dependencies scanner to pick the right candidate when several
    // libraries with the same name exist in the environment.
    for (const auto &symbol : elf->imported_symbols()) {
        info.addRequiredSymbol(formatSymbol(symbol));
    }

    for (const auto &symbol : elf->exported_symbols()) {
        info.addProvidedSymbol(formatSymbol(symbol));
    }

    for (const auto &definition : elf->symbols_version_definition()) {
        for (const auto &aux : definition.symbols_aux()) {
            if (!aux.name().empty()) {
                info.addProvidedVersion(QString::fromStdString(aux.name()));
            }
        }
    }

    return true;
}
