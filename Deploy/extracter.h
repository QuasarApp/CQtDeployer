/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef EXTRACTER_H
#define EXTRACTER_H
#include <QDir>
#include <QString>
#include <QStringList>
#include <dependenciesscanner.h>
#include "deploy_global.h"
#include "filemanager.h"
#include "qml.h"


class DEPLOYSHARED_EXPORT Extracter {
  private:

    QStringList neadedLibs;
    QStringList systemLibs;

    DependenciesScanner scaner;
    FileManager *_fileManager;

    void extract(const QString &file);
    bool copyTranslations(QStringList list);

    bool createQConf();
    bool extractQml();

    QFileInfoList findFilesInsideDir(const QString &name, const QString &dirpath);
    bool extractQmlAll();
    bool extractQmlFromSource(const QString &sourceDir);
    QString filterQmlPath(const QString &path);
    void extractLib(const QString & file);

    bool deployMSVC();


    bool copyPlugin(const QString &plugin);
    void copyPlugins(const QStringList &list);


public:
    explicit Extracter(FileManager *fileManager);
    bool createRunScript(const QString &target);
    void deploy();

    friend class deploytest;
};

#endif // EXTRACTER_H_H
