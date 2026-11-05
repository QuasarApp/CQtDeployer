//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef DMGTEST_H
#define DMGTEST_H

#include <QtTest>
#include <QTemporaryDir>

class QFileInfo;

/**
 * @brief The DmgTest class
 *
 * Unit tests of the "dmg" option (issue #899): the Dmg distribution of
 * the Deploy library (the staging of the DMG volume, the hdiutil command
 * of the runCmd method, the outPutFiles and the removeTemplate method)
 * and the end-to-end creation of the DMG packages by the cqtdeployer of
 * the current build tree.
 *
 * The DMG packages are created with the hdiutil tool of the macOS
 * system (issue #899), so the real DMG creation runs only on the macOS
 * hosts. On the other host platforms the "dmg" option is skipped with a
 * warning (the same rule as the "deb" and the "appimage" options) and
 * the test verifies the warning and the absence of the DMG output.
 */
class DmgTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void optionTableCheck();
    void dmgDistributionStagingCheck();
    void dmgOptionEndToEndCheck();

private:
    void runDeployerProcess(const QStringList &args, const QString &runName,
                            int *exitCode, QString *output);
    QString distDirListing() const;
    static bool dmgIsUdif(const QFileInfo &dmg);

    QTemporaryDir _tmp;
    QString _deployerExe;
};

#endif // DMGTEST_H
