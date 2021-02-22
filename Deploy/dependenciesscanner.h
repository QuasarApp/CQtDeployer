/*
 * Copyright (C) 2018-2021 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef WINDEPENDENCIESSCANNER_H
#define WINDEPENDENCIESSCANNER_H

#include <QMultiMap>
#include <QStringList>
#include "deploy_global.h"
#include "libinfo.h"

class PE;
class ELF;
class GeneralFiles;

enum class PrivateScaner: unsigned char {
   UNKNOWN,
   PE,
   ELF
};

class DEPLOYSHARED_EXPORT DependenciesScanner {


private:

    QMultiHash<QString, QString> _EnvLibs;
    QHash<QString, LibInfo> _scanedLibs;

    PE *_peScaner = nullptr;
    ELF *_elfScaner = nullptr;
    GeneralFiles *_filesScaner = nullptr;

    PrivateScaner getScaner(const QString& lib) const;

    QMultiMap<LibPriority, LibInfo> getLibsFromEnvirement(const QString& libName) const;

    void recursiveDep(LibInfo& lib, QSet<LibInfo> &res, QSet<QString> &libStack);

    void addToWinAPI(const QString& lib, QHash<WinAPI, QSet<QString> > &res);

public:
    explicit DependenciesScanner();
    ~DependenciesScanner();

    void setEnvironment(const QStringList &env);

    QSet<LibInfo> scan(const QString& path);
    bool fillLibInfo(LibInfo& info ,const QString& file) const;


    friend class deploytest;
    void clearScaned();
};

#endif // WINDEPENDENCIESSCANNER_H
