/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef EXTRACTER_H
#define EXTRACTER_H
#include <QDir>
#include <QHash>
#include <QPair>
#include <QString>
#include <QStringList>
#include <dependenciesscanner.h>
#include "dependencymap.h"
#include "deploy_global.h"
#include "filemanager.h"
#include "qmlqt6.h"

class ConfigParser;
class MetaFileManager;
class PluginsParser;

class DEPLOYSHARED_EXPORT Extracter {

public:
    explicit Extracter(FileManager *fileManager, PluginsParser* pluginsParser, ConfigParser * cqt, DependenciesScanner *_scaner);
    bool deploy();
    void clear();

    /**
     * @brief isMachOFile This method return true if the @a file is a Mach-O
     *  binary (32/64-bit in both byte orders and fat/universal binaries).
     *  The check is a full file parsing on the LIEF backend, so the file
     *  must be a valid parseable Mach-O binary (issue #166).
     * @param file This is the path to the checked file.
     * @return true if the file is a Mach-O binary else false.
     */
    static bool isMachOFile(const QString &file);

    /**
     * @brief machORPaths This method return the list of the runtime search
     *  paths (LC_RPATH entries) of all architecture slices of the Mach-O
     *  binary @a file (issue #166). The binary is parsed one time on the
     *  LIEF backend. Because the LIEF 1.0.0 submodule maps LC_RPATH to a
     *  different load command constant than the macOS ABI (0x1C01), the real
     *  entries are read from the raw command data that LIEF keeps for the
     *  unknown commands.
     * @param file This is the path to the Mach-O binary.
     * @return The list of the LC_RPATH entries (empty if the file is not a
     *  Mach-O binary or has no rpaths).
     */
    static QStringList machORPaths(const QString &file);

private:

    QHash<QString, DependencyMap> _packageDependencyes;
    QHash<QString, DeployCore::QtModule> _targetModules;

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
     * @param package This is package id of deployement package.
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
