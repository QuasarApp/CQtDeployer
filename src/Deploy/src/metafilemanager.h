//#
//# Copyright (C) 2018-2026 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef METAFILEMANAGER_H
#define METAFILEMANAGER_H

#include <QString>
#include <deploycore.h>

class FileManager;
class DistroModule;
class TargetInfo;

class MetaFileManager
{


public:
    MetaFileManager(FileManager* manager);

    void createRunMetaFiles(const QHash<QString, DeployCore::QtModule> &modulesMap);


private:
    bool createRunScriptWindows(const QString &target);
    bool createRunScriptLinux(const QString &target);
    QString generateCustoScriptBlok(bool bat) const;

    bool createRunScript(const QString &target);

    bool createQConf(const QString &target);

    QHash<QString, QString> toReplace(const QString &target,
                                      const DistroModule &distro) const;

    void replace(const QHash<QString, QString>& map, QString& content);

    FileManager* _fileManager = nullptr;
    QHash<QString, DeployCore::QtModule> _mudulesMap;
};

#endif // METAFILEMANAGER_H
