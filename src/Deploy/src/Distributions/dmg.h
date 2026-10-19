/*
 * Copyright (C) 2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef DMG_H
#define DMG_H

#include "idistribution.h"

/**
 * @brief The Dmg class
 *
 * Creates a macOS application bundle and a compressed DMG disk image
 * archive for every available package (issue #166), the same way the
 * deb distribution creates a separate .deb file for every available
 * package. Every bundle is staged in the target directory (a copy of
 * the deployed package with the executable files is placed into the
 * Contents/MacOS folder of the bundle) and every disk image is created
 * with the macOS "hdiutil" system tool.
 *
 * The DMG output name of the package is the per-package "dmgOut" value
 * or the default "<name>.dmg" file name (the same way as the "debOut"
 * value of the deb distribution).
 *
 * The distribution is available only on the macOS hosts where the
 * "hdiutil" tool exists. On all other platforms the distribution is
 * skipped with a warning (see the isAvailable method).
 */
class DEPLOYSHARED_EXPORT Dmg: public iDistribution
{
public:
    explicit Dmg(FileManager *fileManager);

    bool deployTemplate(PackageControl &pkgCtrl) override;
    bool deployRawTemplate(PackageControl &pkgCtrl) override;
    bool removeTemplate() const override;
    Envirement toolKitEnv() const override;
    QProcessEnvironment processEnvirement() const override;
    QList<SystemCommandData> runCmd() override;
    QStringList outPutFiles() const override;
    bool isAvailable() const override;

protected:
    QString dataLocation(const DistroModule &module) const override;
    QString location(const DistroModule &module) const override;

private:
    /**
     * @brief bundleName This method return the name of the application
     *  bundle (without the ".app" suffix). By default it is the base
     *  name of the first target of the package with the executable
     *  files, so the name of the bundle matches the name of the
     *  executable.
     * @param module This is the package object.
     * @return bundle name
     */
    QString bundleName(const DistroModule &module) const;

    /**
     * @brief executableName This method return the name of the bundle
     *  executable (the file in the Contents/MacOS folder).
     * @param module This is the package object.
     * @return executable file name
     */
    QString executableName(const DistroModule &module) const;

    /**
     * @brief bundleIdentifier This method builds the CFBundleIdentifier
     *  value for the Info.plist of the bundle.
     * @param bundleName This is the bundle name.
     * @return reverse-domain bundle identifier
     */
    QString bundleIdentifier(const QString &bundleName) const;

    /**
     * @brief rpaths This method return the list of the rpath values that
     *  are added to the bundle executable with the install_name_tool
     *  (issue #166). The @executable_path rpath resolves the @rpath
     *  install names of the executable (the Qt libraries are deployed in
     *  the MacOS folder of the bundle) and the additional rpath is added
     *  on the custom libOut layout.
     * @param module This is the package object.
     * @return rpath values
     */
    QStringList rpaths(const DistroModule &module) const;

    /**
     * @brief moveToBundleRoot This method moves the executable file to
     *  the MacOS root of the bundle (the CFBundleExecutable key must be
     *  the executable file in the MacOS subfolder of the bundle, issue
     *  #166). The move is performed on the bundle copy only, the
     *  original deployed binary is not changed.
     * @param from This is the current executable file location.
     * @param to This is the target location in the MacOS folder.
     * @return true if the file moved successful
     */
    bool moveToBundleRoot(const QString &from, const QString &to) const;

    /**
     * @brief moveQConf This method moves the qt.conf file of the package
     *  to the MacOS root of the bundle (next to the bundle executable)
     *  and updates the Prefix key to the package root (issue #166). On
     *  the default flat deploy layout the qt.conf file is already in the
     *  MacOS root and the method does nothing.
     * @param dataLoc This is the MacOS folder of the bundle.
     * @param module This is the package object.
     * @return true if the qt.conf file moved successful or absent
     */
    bool moveQConf(const QString &dataLoc, const DistroModule &module) const;

    /**
     * @brief createApplicationsLink This method creates the
     *  "Applications" symbolic link in the DMG stage directory. The
     *  link allows to drag the application to the Applications folder
     *  of the user on the mounted image (issue #166).
     * @param stageDir This is the DMG stage directory.
     * @return true if the link created successful
     */
    bool createApplicationsLink(const QString &stageDir) const;

    /**
     * @brief The Stage struct This structure contains data of the
     *  prepared DMG stage (the application bundle of one package,
     *  issue #166).
     */
    struct Stage {
        QString stageDir;    // the directory that will be packed into the DMG archive
        QString executable;  // the bundle executable (Contents/MacOS/<exe>)
        QString volumeName;  // the hdiutil -volname value
        QString output;      // the output .dmg file path
        QStringList rpaths;  // the rpath values of the install_name_tool step
    };

    QList<Stage> _stages;
};

#endif // DMG_H
