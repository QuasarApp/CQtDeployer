//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef APPIMAGETEST_H
#define APPIMAGETEST_H

#include <QtTest>
#include <QTemporaryDir>

class QFileInfo;

/**
 * @brief The AppImageTest class
 *
 * Unit tests of the "appimage" option (issue #898): the option
 * registration in the options table of the DeployCore and the creation of
 * the AppImage packages by the cqtdeployer of the current build tree.
 *
 * The appimagetool is stored in the repository for every Linux platform
 * (issue #898) and is staged next to the built executable by the
 * copyAppImage target, so the AppImage creation tests run on every Linux
 * host: the tool is started directly when the host provides the FUSE
 * support and in the --appimage-extract-and-run mode of the AppImage
 * runtime otherwise (see the wrapper script of the AppImage folder).
 */
class AppImageTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void optionTableCheck();
    void appimagePackageCheck();
    void noQtGeneralFileAppImageCheck();

private:
    static constexpr const char *DEFAULT_APPIMAGE_BASE = "AppImageTest";

    QString appimageName(const char *base = DEFAULT_APPIMAGE_BASE) const;
    QFileInfo appimageInfo(const char *base = DEFAULT_APPIMAGE_BASE) const;
    void runDeployerProcess(const QStringList &args, const QString &runName,
                            int *exitCode, QString *output);
    QString distDirListing() const;
    static bool appImageIsValid(const QFileInfo &appImage);

    QTemporaryDir _tmp;
    QString _deployerExe;
    QString _qmake;
    QString _targetBinary;
    QString _targetLibDir;
};

#endif // APPIMAGETEST_H
