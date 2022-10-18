/*
 * Copyright (C) 2018-2022 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef LIBCREATOR_H
#define LIBCREATOR_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <dependenciesscanner.h>

class LibCreator
{
private:
    QString path;
    QStringList copyedLibs;

    QMap<QString, QStringList> libDep;
    QMap<QString, Platform> libplatform;
    void createLib(const QString& resLib, const QStringList& dep, Platform platform);

    void initLinux64();
    void initWin32();
    void initWin64();
public:
    LibCreator(const QString& path);
    const QStringList &getLibs() const;
    const QMap<QString, QStringList>& getLibsDep() const;
    ~LibCreator();
    const QMap<QString, Platform>& getLibplatform() const;
};

#endif // LIBCREATOR_H
