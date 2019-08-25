
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

public:
    DistroStruct();
    QString getLibOutDir() const;
    void setLibOutDir(const QString &value);
    QString getBinOutDir() const;
    void setBinOutDir(const QString &value);
    QString getQmlOutDir() const;
    void setQmlOutDir(const QString &value);
    QString getTrOutDir() const;
    void setTrOutDir(const QString &value);
    QString getResOutDeir() const;
    void setResOutDeir(const QString &value);
    QString getPluginsOutDir() const;
    void setPluginsOutDir(const QString &value);
};

#endif // DISTROSTRUCT_H
