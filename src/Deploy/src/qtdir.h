/*
 * Copyright (C) 2018-2026 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#ifndef QTDIR_H
#define QTDIR_H

#include <QDir>
#include <humanreadableobject.h>
#include "deploy_global.h"
#include "deploycore.h"

class DEPLOYSHARED_EXPORT QtDir: QuasarAppUtils::iHRO {
    QString libs;
    QString bins;
    QString libexecs;
    QString plugins;
    QString qmls;
    QString translations;
    QString resources;

    Platform _platform = UnknownPlatform;

    QtMajorVersion _qtVersion = NoQt;
public:
    QString getLibs() const;
    void setLibs(const QString &value);
    QString getBins() const;
    void setBins(const QString &value);
    QString getLibexecs() const;
    void setLibexecs(const QString &value);
    QString getPlugins() const;
    void setPlugins(const QString &value);
    QString getQmls() const;
    void setQmls(const QString &value);
    QString getTranslations() const;
    void setTranslations(const QString &value);
    QString getResources() const;
    void setResources(const QString &value);
    Platform getQtPlatform() const;

    /**
     * @brief isQt - This method check a path for belonging to QtDirs.
     * @param path This is cheecked path of library or any qt file.
     * @return true if object is qt.
     */
    bool isQt(QString path) const;

    /**
     * @brief getQtVersion This method return information of version of qt
     * @return the masjor version of qt
     */
    QtMajorVersion getQtVersion() const;

    /**
     * @brief findMingwToolchainBin This method finds the MinGW compiler
     * toolchain directory that is associated with the Qt installation,
     * for example D:/Qt/Tools/mingw1310/bin for the Qt installation in
     * D:/Qt/6.10.3/mingw_64 (the aqt / official installer layout keeps
     * the toolchain in <prefix root>/Tools/mingwXXXX).
     *
     * The MinGW runtime libraries (libstdc++-6.dll, libgcc_s_seh-1.dll,
     * libwinpthread-1.dll) from that toolchain are the only copies that
     * are ABI compatible with the executables and libraries that are
     * built for this Qt. The Qt kit itself ships its own (often older)
     * copies of these runtime libraries in its bin directory, so a
     * deployment that picks the kit copies instead of the toolchain
     * copies crashes at load time with STATUS_ENTRYPOINT_NOT_FOUND
     * (0xC0000139) (issue #861, #862).
     *
     * @param qtBins - The bin directory of the Qt installation,
     *                 for example D:/Qt/6.10.3/mingw_64/bin.
     * @return The bin directory of the MinGW toolchain, for example
     *         D:/Qt/Tools/mingw1310/bin, or an empty string when no
     *         toolchain was found. If several toolchains are found the
     *         one with the highest version number is returned.
     */
    static QString findMingwToolchainBin(const QString &qtBins);

    /**
     * @brief parsePlistVersion This method reads the major version of Qt
     *  from an XML Info.plist of a macOS framework (the
     *  CFBundleShortVersionString or the CFBundleVersion key).
     * @param plistFile This is the path to the Info.plist file.
     * @return 4, 5 or 6 if the version is found else -1.
     */
    static int parsePlistVersion(const QString &plistFile);

    QString toString() const override;
};

#endif // QTDIR_H
