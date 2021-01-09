//#
//# Copyright (C) 2018-2021 QuasarApp.
//# Distributed under the lgplv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef METAFILEMANAGER_H
#define METAFILEMANAGER_H

#include <QString>

class FileManager;

class MetaFileManager
{


public:
    MetaFileManager(FileManager* manager);

    void createRunMetaFiles();


private:
    bool createRunScriptWindows(const QString &target);
    bool createRunScriptLinux(const QString &target);
    QString generateCustoScriptBlok(bool bat) const;

    bool createRunScript(const QString &target);
    bool createQConf(const QString &target);

    FileManager* _fileManager = nullptr;
};

#endif // METAFILEMANAGER_H
