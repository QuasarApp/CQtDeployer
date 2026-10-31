//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef QIFWTEST_H
#define QIFWTEST_H

#include <QtTest>
#include <QTemporaryDir>

class QFileInfo;

/**
 * @brief The QifwTest class
 *
 * Unit tests of the "qtifw" option (the old "qif" name is kept as a
 * deprecated alias, issue #738): the option registration in the options
 * table of the DeployCore and the creation of the Qt Installer Framework
 * installer by the cqtdeployer of the current build tree.
 *
 * The Qt Installer Framework tools are stored in the repository for every
 * supported platform (issue #867) and are staged next to the built
 * executable by the copyQIFW target, so the installer creation tests run
 * on every platform, including the macOS hosts (issue #738).
 */
class QifwTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void optionTableCheck();
    void qtifwInstallerCheck();
    void qifDeprecatedAliasCheck();
    void dmgOptionNotQifCheck();
    void noQtGeneralFileInstallerCheck();

private:
    static constexpr const char *DEFAULT_INSTALLER_BASE = "QifwTestInstaller";

    QString installerName(const char *base = DEFAULT_INSTALLER_BASE) const;
    QFileInfo installerInfo(const char *base = DEFAULT_INSTALLER_BASE) const;
    void runDeployer(const QStringList &distroOptions, const QString &runName,
                     int *exitCode, QString *output);
    void runDeployerProcess(const QStringList &args, const QString &runName,
                            int *exitCode, QString *output);
    QString distDirListing() const;
    QString artifactListing() const;
    bool waitForInstaller(int timeoutMs,
                          const char *base = DEFAULT_INSTALLER_BASE);

    QTemporaryDir _tmp;
    QString _deployerExe;
    QString _qmake;
    QString _targetBinary;
    QString _targetLibDir;
};

#endif // QIFWTEST_H
