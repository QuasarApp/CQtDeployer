//#
//# Copyright (C) 2018-2026 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef LIBINFO_H
#define LIBINFO_H
#include "deploycore.h"


class DEPLOYSHARED_EXPORT LibInfo {

public:

    friend DEPLOYSHARED_EXPORT bool operator == (const LibInfo& left ,const LibInfo& right);
    friend DEPLOYSHARED_EXPORT bool operator > (const LibInfo& left, const LibInfo& right);
    friend DEPLOYSHARED_EXPORT bool operator < (const LibInfo& left, const LibInfo& right);
    friend DEPLOYSHARED_EXPORT bool operator >= (const LibInfo& left, const LibInfo& right);
    friend DEPLOYSHARED_EXPORT bool operator <= (const LibInfo& left, const LibInfo& right);

    QString fullPath() const;

    void clear();

    bool isValid() const;
    bool isScaned() const;

    const QSet<LibInfo>& getAllDep() const;
    Platform getPlatform() const;
    void setPlatform(const Platform &value);
    QString getName() const;
    void setName(const QString &value);
    QString getPath() const;
    void setPath(const QString &value);
    QSet<QString> getDependncies() const;
    void setDependncies(const QSet<QString> &value);
    void addDependncies(const QString &value);
    void addDepends(const QString &value);
    void addDependncies(const QSet<QString> &value);
    void removeDependncies(const QString &value);

    /// Symbols imported by this library (from the import table / delay
    /// import table of a PE, the undefined dynamic symbols of an ELF or the
    /// imported symbols of a Mach-O binary). A versioned symbol is stored
    /// as "symbol@version" (ELF symbol versions, e.g.
    /// "__cxa_allocate_exception@GLIBCXX_3.4.31"), an unversioned symbol as
    /// its plain name.
    const QSet<QString>& getRequiredSymbols() const;
    void addRequiredSymbol(const QString &value);
    void addRequiredSymbols(const QSet<QString> &value);

    /// Symbols exported by this library (PE export table, defined ELF
    /// dynamic symbols, defined Mach-O symbols). Same "symbol@version"
    /// format as getRequiredSymbols().
    const QSet<QString>& getProvidedSymbols() const;
    void addProvidedSymbol(const QString &value);
    void addProvidedSymbols(const QSet<QString> &value);

    /// Version nodes defined by this library (ELF only, the
    /// `.gnu.version_d` / DT_VERDEF names, e.g. "GLIBCXX_3.4.31").
    const QSet<QString>& getProvidedVersions() const;
    void addProvidedVersion(const QString &value);
    void addProvidedVersions(const QSet<QString> &value);

    LibPriority getPriority() const;
    void setPriority(const LibPriority &value);
    const QString& getRPath() const;
    void setRPath(const QString &value);
    WinAPI getWinApi() const;
    void setWinApi(WinAPI winApi);
    QtMajorVersion isDependetOfQt() const;

    QString toString() const;

    friend class DependenciesScanner;

private:
    QSet<LibInfo> _allDep;

    Platform _platform = Platform::UnknownPlatform;
    QString _name;
    QString _path;
    QSet<QString> _dependncies;
    QSet<QString> _requiredSymbols;
    QSet<QString> _providedSymbols;
    QSet<QString> _providedVersions;
    bool _dependsOfGui;
    QString _rpath;
    LibPriority _priority = NotFile;
    WinAPI _winApi = WinAPI::NoWinAPI;

};

DEPLOYSHARED_EXPORT uint qHash(const LibInfo& info);
#endif // LIBINFO_H
