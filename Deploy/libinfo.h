//#
//# Copyright (C) 2018-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef LIBINFO_H
#define LIBINFO_H
#include "deploycore.h"


class DEPLOYSHARED_EXPORT LibInfo {

public:

    friend bool operator == (const LibInfo& left ,const LibInfo& right);
    friend bool operator > (const LibInfo& left, const LibInfo& right);
    friend bool operator < (const LibInfo& left, const LibInfo& right);
    friend bool operator >= (const LibInfo& left, const LibInfo& right);
    friend bool operator <= (const LibInfo& left, const LibInfo& right);

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
    void addDependncies(const QSet<QString> &value);
    void removeDependncies(const QString &value);

    LibPriority getPriority() const;
    void setPriority(const LibPriority &value);
    QString getQtPath() const;
    void setQtPath(const QString &value);
    WinAPI getWinApi() const;
    void setWinApi(WinAPI winApi);
    QtMajorVersion isDependetOfQt() const;

    friend class DependenciesScanner;

private:
    QSet<LibInfo> _allDep;

    Platform _platform = Platform::UnknownPlatform;
    QString _name;
    QString _path;
    QSet<QString> _dependncies;
    bool _dependsOfGui;
    QString _qtPath;
    LibPriority _priority = NotFile;
    WinAPI _winApi = WinAPI::NoWinAPI;

};

uint qHash(const LibInfo& info);
#endif // LIBINFO_H
