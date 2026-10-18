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
 * the Mach-O magic check, the LC_RPATH reader, the .framework layout
 * helpers of the DeployCore and the install name change planner of the
 * Extracter. The tests use hand built Mach-O binaries, so they run on
 * every platform.
 */
class MacOSTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void machOMagicCheck();
    void frameworkHelpers();
    void machORPathsCheck();
    void normalizeMacOSRPathCheck();
    void planMacOSChangesCheck();

private:
    QTemporaryDir _tmp;
};

#endif // MACOSTEST_H