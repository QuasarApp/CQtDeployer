/*
 * Copyright (C) 2018-2019 QuasarApp.
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
    auto exports = internal->exports;

    std::set<std::string> filter;

    for ( auto &i : imports) {
        if (!filter.count(i.moduleName)) {
            filter.insert(i.moduleName);
            res.addDependncies(QString::fromStdString(i.moduleName));
        }
    }

    for ( auto &i : exports) {
        if (!filter.count(i.moduleName)) {
            filter.insert(i.moduleName);
            res.addDependncies(QString::fromStdString(i.moduleName));
        }
    }

    if (res.getName().contains(APU_MS_WIN, Qt::CaseInsensitive)) {
        res.addDependncies(_apimswin);
    }

    return res.getDependncies().size() || !imports.size();
}

PE::PE(): IGetLibInfo () {

}

void PE::setWinApiPlugins(const QSet<QString> &keys) {
    _apimswin = keys;
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

    if (!getDep(parsedPeLib->internal, info)) {
        peparse::DestructParsedPE(parsedPeLib);
        return false;
    }

    peparse::DestructParsedPE(parsedPeLib);

    return info.isValid();
}

PE::~PE(){

}
