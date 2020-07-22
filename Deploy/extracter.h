/*
 * Copyright (C) 2018-2020 QuasarApp.
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
    void deploy();
    void clear();

private:

    QHash<QString, DependencyMap> _packageDependencyes;

    DependenciesScanner *_scaner;
    FileManager *_fileManager;
    PluginsParser* _pluginsParser;
    ConfigParser *_cqt;
    MetaFileManager *_metaFileManager;

    void extract(const QString &file, DependencyMap* depMap, const QString& mask = "");
    bool copyTranslations(const QStringList &list, const QString &package);

    bool extractQml();

    QFileInfoList findFilesInsideDir(const QString &name, const QString &dirpath);
    bool extractQmlAll();
    bool extractQmlFromSource();
    /**
   * @brief extractLib
   * @param file file of lib
   * @param mask  extraction mask. Used to filter extracts objects
   */
    void extractLib(const QString & file, DependencyMap *depMap, const QString& mask = "");

    bool deployMSVC();
    bool extractWebEngine();

    /**
     * @brief angleGLLibs -
     *  @note see task 398 (https://github.com/QuasarApp/CQtDeployer/issues/398)
     * @return
     */
    QList<QString> angleGLLibs();

    /**
   * @brief compress - this function join all target dependecies in to one struct
   */
    void compress();
    void extractAllTargets();
    void extractPlugins();

    void copyFiles();
    void copyTr();
    void copyLibs(const QSet<QString> &files, const QString &package);

    bool isWebEngine(const QString& package) const;
    void extractPluginLib(const QString &item, const QString &package);

    friend class deploytest;
    void copyExtraPlugins(const QString &package);
};

#endif // EXTRACTER_H_H
