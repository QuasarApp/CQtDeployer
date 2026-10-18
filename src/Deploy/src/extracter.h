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

namespace LIEF {
namespace MachO {
class FatBinary;
}
}

class ConfigParser;
class MetaFileManager;
class PluginsParser;

class DEPLOYSHARED_EXPORT Extracter {

public:
    explicit Extracter(FileManager *fileManager, PluginsParser* pluginsParser, ConfigParser * cqt, DependenciesScanner *_scaner);
    bool deploy();
    void clear();

    /**
     * @brief The MacOSFileChanges struct This struct contain the list of
     *  changes that must be applied to a Mach-O binary so that it resolves
     *  its libraries from the deployed directory (issue #166):
     *  1. the LC_LOAD_DYLIB references of the needed libraries are rewritten
     *     from the original install names (for example the absolute paths of
     *     the Qt installation) to the @rpath references of the deployed
     *     layout (changedLibs),
     *  2. the LC_ID_DYLIB install name of the binary itself is set to its
     *     new @rpath location, when the binary is one of the deployed
     *     libraries (newInstallName),
     *  3. the LC_RPATH entries that make the @rpath references resolvable
     *     are added (addedRPaths).
     */
    struct MacOSFileChanges {
        /// The new LC_ID_DYLIB install name of the binary. Empty if unchanged.
        QString newInstallName;

        /// The LC_LOAD_DYLIB names that must be rewritten (old -> new).
        QList<QPair<QString, QString>> changedLibs;

        /// The LC_RPATH entries that must be added to the binary.
        QStringList addedRPaths;
    };

    /**
     * @brief isMacOSFile This method return true if the @a file is a Mach-O
     *  binary (32/64-bit in both byte orders and fat/universal binaries).
     *  The check is a full file parsing on the LIEF backend, so the file
     *  must be a valid parseable Mach-O binary (issue #166).
     * @param file This is the path to the checked file.
     * @return true if the file is a Mach-O binary else false.
     */
    static bool isMacOSFile(const QString &file);

    /**
     * @brief macOSRPaths This method return the list of the runtime search
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
    static QStringList macOSRPaths(const QString &file);

    /**
     * @brief planMacOSChanges This method calculate the changes that are
     *  needed to make the Mach-O binary @a binary (the file @a file) resolve
     *  its libraries from the deployed directory (issue #166).
     *  Use this overload for the binaries parsed with
     *  DeployCore::parseMacOSFile(), so the file is not parsed a second time
     *  (the full LIEF parsing is a heavy operation and every file must be
     *  parsed only one time).
     * @param binary This is the parse result of the LIEF Mach-O parser.
     * @param file This is the path to the Mach-O binary.
     * @param references This is a map of the normalized library names
     *  (see DeployCore::normalizeMacOSLibName) to the dyld references that
     *  must be used in the deployed layout (@rpath/...).
     * @param libOutDir This is the absolute path to the directory with the
     *  deployed libraries.
     * @return The list of the needed changes (empty if the binary is not a
     *  valid Mach-O binary).
     */
    static MacOSFileChanges planMacOSChanges(const LIEF::MachO::FatBinary *binary,
                                             const QString &file,
                                             const QHash<QString, QString> &references,
                                             const QString &libOutDir);

    /**
     * @brief planMacOSChanges This method calculate the changes that are
     *  needed to make the Mach-O binary @a file resolve its libraries from
     *  the deployed directory (issue #166). The binary is parsed one time on
     *  the LIEF backend with DeployCore::parseMacOSFile().
     * @param file This is the path to the Mach-O binary.
     * @param references This is a map of the normalized library names
     *  (see DeployCore::normalizeMacOSLibName) to the dyld references that
     *  must be used in the deployed layout (@rpath/...).
     * @param libOutDir This is the absolute path to the directory with the
     *  deployed libraries.
     * @return The list of the needed changes (empty if the file is not a
     *  valid Mach-O binary).
     */
    static MacOSFileChanges planMacOSChanges(const QString &file,
                                             const QHash<QString, QString> &references,
                                             const QString &libOutDir);

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
     * @brief deployMacOSRPaths This method rewrite the Mach-O install names
     *  (LC_ID_DYLIB / LC_LOAD_DYLIB) and the runtime search paths (LC_RPATH)
     *  of all deployed binaries so that the libraries are resolved from the
     *  deployed directory. On macOS the changes are applied with the
     *  install_name_tool and the binaries are signed again with an ad-hoc
     *  signature (issue #166).
     * @return true if all Mach-O binaries are fixed else false.
     */
    bool deployMacOSRPaths();

    /**
     * @brief fixMacOSWithInstallNameTool This method apply the @a changes to
     *  the Mach-O binary @a file with the install_name_tool and sign the
     *  result with an ad-hoc signature (issue #166).
     * @param file This is the path to the Mach-O binary.
     * @param changes This is the list of the needed changes.
     * @return true if all changes are applied else false.
     */
    bool fixMacOSWithInstallNameTool(const QString &file, const MacOSFileChanges &changes);

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
