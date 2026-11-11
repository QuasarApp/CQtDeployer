//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef DEBTEST_H
#define DEBTEST_H

#include <QtTest>
#include <QTemporaryDir>

class QFileInfo;

/**
 * @brief The DebTest class
 *
 * Unit tests of the "deb" option: the option registration in the options
 * table of the DeployCore and the creation of the debian package by the
 * cqtdeployer of the current build tree. The -license option of the
 * package is verified in the content of the created deb package (the
 * license file is stored in the usr/share/doc/<package>/copyright file of
 * the Debian policy, issue #456).
 *
 * The deb option is available on the Linux host platforms only (the
 * dpkg-deb tool of the Linux system), on the other platforms the package
 * creation tests are skipped.
 */
class DebTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void optionTableCheck();
    void debLicenseCheck();

private:
    static constexpr const char *DEB_BASE = "DebLicenseCheck";

    QString debName(const char *base = DEB_BASE) const;
    QFileInfo debInfo(const char *base = DEB_BASE) const;
    QString packageName() const;
    void runDeployerProcess(const QStringList &args, const QString &runName,
                            int *exitCode, QString *output);
    QString distDirListing() const;
    bool dpkgDebAvailable() const;

    QTemporaryDir _tmp;
    QString _deployerExe;
    QString _qmake;
    QString _targetBinary;
    QString _targetLibDir;
};

#endif // DEBTEST_H