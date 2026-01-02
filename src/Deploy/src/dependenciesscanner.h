/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef WINDEPENDENCIESSCANNER_H
#define WINDEPENDENCIESSCANNER_H

#include <QMultiMap>
#include <QStringList>
#include "deploy_global.h"
#include "pe_type.h"
#include "elf_type.h"
#include "libinfo.h"
#include "generalfiles_type.h"


enum class PrivateScaner: unsigned char {
   UNKNOWN,
   PE,
   ELF
};

class DEPLOYSHARED_EXPORT DependenciesScanner {


private:

    QMultiHash<QString, QString> _EnvLibs;
    QHash<QString, LibInfo> _scanedLibs;

    PE _peScaner;
    ELF _elfScaner;
    GeneralFiles _filesScaner;

    PrivateScaner getScaner(const QString& lib) const;

    QMultiMap<LibPriority, LibInfo> getLibsFromEnvirement(const QString& libName) const;

    void recursiveDep(LibInfo& lib, QSet<LibInfo> &res, QSet<QString> &libStack);

    void addToWinAPI(const QString& lib, QHash<WinAPI, QSet<QString> > &res);

public:
    explicit DependenciesScanner();

    void setEnvironment(const QStringList &env);

    /**
     * @brief scan This method create a "lib info" object from path and extract all depends on from the current environment.
     * @param path This is full path to the library
     * @return full lib of executable info
     */
    LibInfo scan(const QString& path);

    /**
     * @brief scan This method do same as a scan(const QString& path) but not use already created libInfo data.
     * @param lib this is already prepared lib info.
     */
    void scan(LibInfo& lib);

    bool fillLibInfo(LibInfo& info, const QString& file) const;

    ~DependenciesScanner();

    friend class deploytest;
    void clearScaned();
};

#endif // WINDEPENDENCIESSCANNER_H
