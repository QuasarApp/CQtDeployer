/*
 * Copyright (C) 2018-2020 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include "pe.h"

#include <QFile>
#include <QFileInfo>
#include <QSet>
#include <QVector>
#include <parser-library/parse.h>
#include <quasarapp.h>

#include <bits/stl_set.h>

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

    if (static_cast<RunType>(parsedPeLib->peHeader.nt.OptionalMagic) == RunType::_32bit) {
        info.setPlatform(Platform::Win32);
    } else if (static_cast<RunType>(parsedPeLib->peHeader.nt.OptionalMagic) == RunType::_64bit) {
        info.setPlatform(Platform::Win64);
    } else {
        info.setPlatform(Platform::UnknownPlatform);
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
