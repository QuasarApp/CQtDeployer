/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "pe_type.h"

#include <QFile>
#include <QFileInfo>
#include <QSet>
#include <QVector>
#include <pe-parse/parse.h>
#include <quasarapp.h>

#include <set>

namespace peparse {

class section;

struct importent {
  VA addr;
  std::string symbolName;
  std::string moduleName;
};

struct exportent {
  VA addr;
  std::string symbolName;
  std::string moduleName;
};

class reloc;
class symbol;

struct parsed_pe_internal {
  std::vector<section> secs;
  std::vector<resource> rsrcs;
  std::vector<importent> imports;
  std::vector<reloc> relocs;
  std::vector<exportent> exports;
  std::vector<symbol> symbols;
};

}

bool PE::getDep(peparse::parsed_pe_internal * internal, LibInfo &res) const {
    auto imports = internal->imports;
    std::set<std::string> filter;

    for ( auto &i : imports) {
        if (!filter.count(i.moduleName)) {
            filter.insert(i.moduleName);
            res.addDependncies(QString::fromStdString(i.moduleName));
        }
    }

    if (res.getWinApi() != WinAPI::NoWinAPI) {
        res.addDependncies(_winAPI.value(res.getWinApi()));
    }

    return res.getDependncies().size() || !imports.size();
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
    auto parsedPeLib = peparse::ParsePEFromFile(lib.toLatin1());

    if (!parsedPeLib)
        return false;

    if (parsedPeLib->peHeader.nt.FileHeader.Machine == peparse::IMAGE_FILE_MACHINE_ARM ||
        parsedPeLib->peHeader.nt.FileHeader.Machine == peparse::IMAGE_FILE_MACHINE_ARM64 ||
        parsedPeLib->peHeader.nt.FileHeader.Machine == peparse::IMAGE_FILE_MACHINE_ARMNT) {

        if (parsedPeLib->peHeader.nt.OptionalMagic == peparse::NT_OPTIONAL_32_MAGIC) {
            info.setPlatform(Win_ARM_32);

        } else {
            info.setPlatform(win_ARM_64);
        }

    } else if (parsedPeLib->peHeader.nt.FileHeader.Machine == peparse::IMAGE_FILE_MACHINE_I386 ||
               parsedPeLib->peHeader.nt.FileHeader.Machine == peparse::IMAGE_FILE_MACHINE_AMD64) {

        if (parsedPeLib->peHeader.nt.OptionalMagic == peparse::NT_OPTIONAL_32_MAGIC) {
            info.setPlatform(Win32);

        } else {
            info.setPlatform(Win64);
        }

    } else {
        info.setPlatform(UnknownPlatform);
        return false;
    }

    info.setName(QFileInfo(lib).fileName());
    info.setPath(QFileInfo(lib).absolutePath());
    info.setWinApi(getAPIModule(info.getName()));

    if (!getDep(parsedPeLib->internal, info)) {
        peparse::DestructParsedPE(parsedPeLib);
        return false;
    }

    peparse::DestructParsedPE(parsedPeLib);


    return info.isValid();
}

PE::~PE(){

}
