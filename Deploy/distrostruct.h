
//#
//# Copyright (C) 2018-2019 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef DISTROSTRUCT_H
#define DISTROSTRUCT_H

#include <QString>

/**
 * @brief The DistroStruct class
 */
class DistroStruct
{
private:
    QString libOutDir;
    QString binOutDir;
    QString qmlOutDir;
    QString trOutDir;
    QString resOutDeir;
    QString pluginsOutDir;

    QString getCanonicalPath(QString path) const;

public:
    DistroStruct();
    QString getLibOutDir(const QString& basePath = "/") const;
    void setLibOutDir(const QString &value);
    QString getBinOutDir(const QString& basePath = "/") const;
    void setBinOutDir(const QString &value);
    QString getQmlOutDir(const QString& basePath = "/") const;
    void setQmlOutDir(const QString &value);
    QString getTrOutDir(const QString& basePath = "/") const;
    void setTrOutDir(const QString &value);
    QString getResOutDeir(const QString& basePath = "/") const;
    void setResOutDeir(const QString &value);
    QString getPluginsOutDir(const QString& basePath = "/") const;
    void setPluginsOutDir(const QString &value);
};

#endif // DISTROSTRUCT_H
