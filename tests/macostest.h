//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef MACOSTEST_H
#define MACOSTEST_H

#include <QtTest>
#include <QTemporaryDir>

/**
 * @brief The MacOSTest class
 *
 * Unit tests for the macOS (Mach-O) deployment helpers (issue #166):
 * the Mach-O type detection, the LC_RPATH reader and the .framework
 * layout helpers of the DeployCore. The tests use hand built Mach-O
 * binaries, so they run on every platform.
 */
class MacOSTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void machODetectionCheck();
    void frameworkHelpers();
    void machORPathsCheck();
    void dmgDistributionCheck();
    void dmgOutConfigCheck();
    void dmgCreationCheck();

private:
    QTemporaryDir _tmp;
};

#endif // MACOSTEST_H