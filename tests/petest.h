//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#ifndef PETEST_H
#define PETEST_H

#include <QtTest>

/**
 * @brief The PeTest class
 *
 * Fast unit tests for the pe-parse library integration (issue #861).
 * Uses the example.exe fixture from the pe-parse submodule, whose
 * path is provided by CMake as the PE_EXAMPLE_EXE compile definition.
 */
class PeTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void parseValidFile();
    void parseInvalidFile();
    void parseViaDeployWrapper();

private:
    QString m_exampleExe;
};

#endif // PETEST_H