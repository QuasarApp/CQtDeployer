
//#
//# Copyright (C) 2018-2019 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef DISTROSTRUCT_H
#define DISTROSTRUCT_H

#include <QString>
#include <deploy_global.h>

/**
 * @brief The DistroStruct class
 */
class DEPLOYSHARED_EXPORT DistroStruct
{
private:
    QString libOutDir;
    QString binOutDir;
    QString qmlOutDir;
    QString trOutDir;
    QString resOutDir;
    QString pluginsOutDir;

    QString getRelativePath(QString path) const;
    QString fixPath(const QString& path) const;

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
    QString getResOutDir(const QString& basePath = "/") const;
    void setResOutDir(const QString &value);
    QString getPluginsOutDir(const QString& basePath = "/") const;
    void setPluginsOutDir(const QString &value);
    QString getRootDir(const QString& basePath = "/") const;

    friend class deploytest;
};

#endif // DISTROSTRUCT_H
