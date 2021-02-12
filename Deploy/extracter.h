/*
 * Copyright (C) 2018-2021 QuasarApp.
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
#include "dependencymap.h"
#include "deploy_global.h"
#include "filemanager.h"
#include "qml.h"

class ConfigParser;
class MetaFileManager;
class PluginsParser;

class DEPLOYSHARED_EXPORT Extracter {

public:
    explicit Extracter(FileManager *fileManager, PluginsParser* pluginsParser, ConfigParser * cqt, DependenciesScanner *_scaner);
    bool deploy();
    void clear();

private:

    QHash<QString, DependencyMap> _packageDependencyes;

    DependenciesScanner *_scaner;
    FileManager *_fileManager;
    PluginsParser* _pluginsParser;
    ConfigParser *_cqt;
    MetaFileManager *_metaFileManager;

    void extract(const QString &file, DependencyMap* depMap, const QString& mask = "");
    void extractExtraData(DependencyMap* depMap, const QString& mask = "");

    bool copyTranslations(const QStringList &list, const QString &package);

    bool extractQml();

    QFileInfoList findFilesInsideDir(const QString &name, const QString &dirpath);
    /**
   * @brief extractLib
   * @param file file of lib
   * @param mask  extraction mask. Used to filter extracts objects
   */
    void extractLib(const QString & file, DependencyMap *depMap, const QString& mask = "");

    bool deployMSVC();
    bool extractWebEngine();

    /**
     * @brief angleGLLibs This method return the list of not dependent libs but needed wor working of web Engine (Windows only).
     *  @note see task 398 (https://github.com/QuasarApp/CQtDeployer/issues/398)
     * @return
     */
    QList<QString> angleGLLibs();

    /**
   * @brief compress - this function join all target dependecies in to one struct
   */
    void compress();
    void extractAllTargets();
    void extractExtraDataTargets();

    void extractPlugins();

    void copyFiles();
    bool copyTr();

    /**
     * @brief copyLibs This method copy input libraryes into libOut dir.
     * @param files This is set of input libs
     * @param package This is package id of deployement pacakge.
     * @param system This option set for systems libraryes. If This option will be set to true then libOut dir will be changed to libOut/systemLibs. see https://github.com/QuasarApp/CQtDeployer/issues/396.
     */
    void copyLibs(const QSet<QString> &files, const QString &package, bool system);
    void copyExtraData(const QSet<QString> &files, const QString &package);

    bool isWebEngine(const QString& package) const;
    void extractPluginLib(const QString &item, const QString &package);

    friend class deploytest;
    void copyExtraPlugins(const QString &package);
};

#endif // EXTRACTER_H_H
