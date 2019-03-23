#include "pe.h"

#include <QFile>
#include <QFileInfo>
#include <QSet>
#include <QVector>
#include <parse.h>

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

bool PE::getDep(peparse::parsed_pe_internal * internal, QStringList &res) {
    auto imports = internal->imports;

    std::set<std::string> filter;

    for ( auto &i : imports) {
        if (!filter.count(i.moduleName)) {
            filter.insert(i.moduleName);
            res.push_back(QString::fromStdString(i.moduleName));
        }
    }

    return res.size();
}

PE::PE(): IGetLibInfo () {

}

bool PE::getLibInfo(const QString &lib, LibInfo &info) {
    auto parsedPeLib = peparse::ParsePEFromFile(lib.toLatin1());

    if (static_cast<RunType>(parsedPeLib->peHeader.nt.OptionalMagic) == RunType::_32bit) {
        info.platform = Platform::Win32;
    } else if (static_cast<RunType>(parsedPeLib->peHeader.nt.OptionalMagic) == RunType::_64bit) {
        info.platform = Platform::Win64;
    } else {
        info.platform = Platform::UnknownPlatform;
    }

    info.name = QFileInfo(lib).fileName();
    info.path = QFileInfo(lib).absolutePath();

    if (!getDep(parsedPeLib->internal, info.dependncies)) {
        return false;
    }

    peparse::DestructParsedPE(parsedPeLib);

    return info.isValid();
}

PE::~PE(){

}
