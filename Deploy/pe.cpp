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

class reloc;
class exportent;
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

    return res.getDependncies().size();
}

PE::PE(): IGetLibInfo () {

}

bool PE::getLibInfo(const QString &lib, LibInfo &info) const {
    auto parsedPeLib = peparse::ParsePEFromFile(lib.toLatin1());

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
        return false;
    }

    peparse::DestructParsedPE(parsedPeLib);

    return info.isValid();
}

PE::~PE(){

}
