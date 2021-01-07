
//#
//# Copyright (C) 2018-2021 QuasarApp.
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
    QString extraDataOutDir;
    QString libOutDir;
    QString binOutDir;
    QString qmlOutDir;
    QString trOutDir;
    QString resOutDir;
    QString pluginsOutDir;

    QString toFullPath(QString path) const;
    QString stripPath(QString path) const;
    QString getRelativePath(QString path) const;


public:
    DistroStruct();

    void setBinOutDir(const QString &value);
    void setLibOutDir(const QString &value);
    void setQmlOutDir(const QString &value);
    void setTrOutDir(const QString &value);
    void setResOutDir(const QString &value);
    void setPluginsOutDir(const QString &value);
    void setExtraDataOutDir(const QString &value);

    QString getLibOutDir(const QString& basePath = "/") const;
    QString getBinOutDir(const QString& basePath = "/") const;
    QString getQmlOutDir(const QString& basePath = "/") const;
    QString getTrOutDir(const QString& basePath = "/") const;
    QString getResOutDir(const QString& basePath = "/") const;
    QString getPluginsOutDir(const QString& basePath = "/") const;
    QString getRootDir(const QString& basePath = "/") const;
    QString getExtraDataOutDir(const QString& basePath = "/") const;

    bool isEmpty();

    friend class deploytest;

};

#endif // DISTROSTRUCT_H
