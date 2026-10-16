/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "pe_type.h"

#include <QFile>
#include <QFileInfo>
#include <QSet>
#include <QVector>
#include <quasarapp.h>

#include <LIEF/PE.hpp>

namespace {

/// Collect all imported module names (the standard import table plus the
/// delay import directory).
QSet<QString> collectImports(const LIEF::PE::Binary *pe) {
    QSet<QString> modules;

    for (const auto &import : pe->imports()) {
        if (!import.name().empty()) {
            modules.insert(QString::fromStdString(import.name()));
        }
    }

    for (const auto &delayImport : pe->delay_imports()) {
        if (!delayImport.name().empty()) {
            modules.insert(QString::fromStdString(delayImport.name()));
        }
    }

    return modules;
}

} // namespace

bool PE::getDep(const QSet<QString> &imports, LibInfo &res) const {
    for (const auto &module : imports) {
        res.addDependncies(module);
    }

    if (res.getWinApi() != WinAPI::NoWinAPI) {
        res.addDependncies(_winAPI.value(res.getWinApi()));
    }

    return res.getDependncies().size() || !imports.size();
}

void PE::refineWindowsPlatform(const QSet<QString> &imports,
                               const QString &libName,
                               LibInfo &res) const {
    auto platform = res.getPlatform();
    if (platform != Win32 && platform != Win64)
        return;

    // issue #855: Windows binaries are split into runtime families by their
    // import table, so the deployer selects a compatible runtime set:
    //  - MinGW binaries linked against the MSVC CRT import msvcrt.dll and
    //    the MinGW runtime (libgcc_s_* / libwinpthread-* / libstdc++-*)
    //    and get the *_MinGW_Msvcrt platform;
    //  - MinGW binaries linked against the UCRT import api-ms-win-crt-*
    //    modules and get the *_MinGW_Ucrt platform;
    //  - MSVC toolchain binaries import the MSVC CRT (msvcr*.dll) and get
    //    the *_MSVC platform;
    //  - everything else keeps the generic platform. UCRT MSVC binaries
    //    (vcruntime140.dll + api-ms-win-crt-*) cannot be told apart from
    //    other UCRT binaries by their import table, and system DLLs import
    //    vcruntime140.dll too, so vcruntime140.dll is not an MSVC marker.
    const QString msvcrtCrt = QStringLiteral("msvcrt.dll");
    const QString ucrtPrefix = QStringLiteral("api-ms-win-crt-");
    const QString msvcCrtPrefix = QStringLiteral("msvcr");

    bool importMsvcrt = false;
    bool importUcrt = false;
    bool importMinGW = false;
    bool importMsvcCrt = false;
    for (const auto &mod : imports) {
        if (!importMsvcrt && mod.compare(msvcrtCrt, Qt::CaseInsensitive) == 0)
            importMsvcrt = true;
        if (!importUcrt && mod.startsWith(ucrtPrefix, Qt::CaseInsensitive))
            importUcrt = true;
        if (!importMinGW && (mod.startsWith("libgcc_s_", Qt::CaseInsensitive) ||
                             mod.startsWith("libwinpthread-", Qt::CaseInsensitive) ||
                             mod.startsWith("libstdc++-", Qt::CaseInsensitive)))
            importMinGW = true;
        // msvcr<version>.dll (msvcr100/120/140/140_1/...) is the MSVC CRT;
        // the isDigit() check keeps msvcrt.dll out of this group.
        if (!importMsvcCrt && mod.startsWith(msvcCrtPrefix, Qt::CaseInsensitive) &&
            mod.size() > msvcCrtPrefix.size() && mod.at(msvcCrtPrefix.size()).isDigit())
            importMsvcCrt = true;
        if (importMsvcrt && importUcrt && importMinGW && importMsvcCrt)
            break;
    }

    // The MinGW runtime libraries themselves (libstdc++-6.dll, libgcc_s_*.dll,
    // libwinpthread-1.dll) are recognized by their file name.
    const bool isMinGWLib = libName.startsWith("libgcc_s_", Qt::CaseInsensitive) ||
                            libName.startsWith("libwinpthread-", Qt::CaseInsensitive) ||
                            libName.startsWith("libstdc++-", Qt::CaseInsensitive);

    if (importMinGW || isMinGWLib) {
        if (importMsvcrt && !importUcrt) {
            res.setPlatform(platform == Win64 ? Win64_MinGW_Msvcrt : Win32_MinGW_Msvcrt);
        } else if (importUcrt && !importMsvcrt) {
            res.setPlatform(platform == Win64 ? Win64_MinGW_Ucrt : Win32_MinGW_Ucrt);
        }
    } else if (importMsvcCrt) {
        res.setPlatform(platform == Win64 ? Win64_MSVC : Win32_MSVC);
    }
}

QHash<WinAPI, QSet<QString> > PE::getWinAPI() const {
    return _winAPI;
}

void PE::setWinAPI(const QHash<WinAPI, QSet<QString> > &winAPI) {
    _winAPI = winAPI;
}

WinAPI PE::getAPIModule(const QString &libName) const {
    if (libName.contains(API_MS_WIN, Qt::CaseInsensitive)) {
        if (libName.contains(API_MS_WIN_CORE, Qt::CaseInsensitive)) {
            return WinAPI::Core;
        }

        if (libName.contains(API_MS_WIN_EVENTING, Qt::CaseInsensitive)) {
            return WinAPI::Eventing;
        }

        if (libName.contains(API_MS_WIN_DEVICES, Qt::CaseInsensitive)) {
            return WinAPI::Devices;
        }

        if (libName.contains(API_MS_WIN_CRT, Qt::CaseInsensitive)) {
            return WinAPI::Crt;
        }

        if (libName.contains(API_MS_WIN_SECURITY, Qt::CaseInsensitive)) {
            return WinAPI::Security;
        }

        if (libName.contains(API_MS_WIN_BASE, Qt::CaseInsensitive)) {
            return WinAPI::Base;
        }

        return WinAPI::Other;

    }

    return WinAPI::NoWinAPI;
}

PE::PE(): IGetLibInfo () {

}

bool PE::getLibInfo(const QString &lib, LibInfo &info) const {
    QFileInfo infolib(lib);
    if (!infolib.exists()) {
        return false;
    }

    auto pe = LIEF::PE::Parser::parse(infolib.absoluteFilePath().toStdString());
    if (pe == nullptr) {
        return false;
    }

    const auto machine = pe->header().machine();
    const bool is64 = pe->optional_header().magic() == LIEF::PE::PE_TYPE::PE32_PLUS;

    if (LIEF::PE::Header::is_arm(machine) || LIEF::PE::Header::is_arm64(machine)) {
        info.setPlatform(is64 ? Win_ARM_64 : Win_ARM_32);

    } else if (LIEF::PE::Header::x86(machine) || LIEF::PE::Header::x86_64(machine)) {
        info.setPlatform(is64 ? Win64 : Win32);

    } else {
        info.setPlatform(UnknownPlatform);
        return false;
    }

    info.setName(infolib.fileName());
    info.setPath(infolib.absolutePath());

    auto imports = collectImports(pe.get());

    refineWindowsPlatform(imports, info.getName(), info);

    info.setWinApi(getAPIModule(info.getName()));

    if (!getDep(imports, info)) {
        return false;
    }

    return info.isValid();
}

PE::~PE(){

}
