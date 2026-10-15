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
 *
 * Also checks the Windows runtime platform detection performed during
 * the PE parsing stage (issue #855). Minimal PE32/PE32+ fixtures are
 * generated at runtime, so no binary fixtures are checked in:
 *  - a MinGW binary linked against the MSVC CRT imports msvcrt.dll and
 *    gets the *_MinGW_Msvcrt platform;
 *  - a MinGW binary linked against the UCRT imports api-ms-win-crt-*
 *    modules and gets the *_MinGW_Ucrt platform;
 *  - a MSVC toolchain binary imports the MSVC CRT (msvcr*.dll) and gets
 *    the *_MSVC platform;
 *  - other binaries (including UCRT MSVC binaries) keep the generic
 *    platform.
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

    void classifyMinGWFlavors();
    void classifyMSVCFlavor();
    void classifyPrecedenceAndGeneric();
    void recognizeMinGWRuntimes();

private:
    QString m_exampleExe;
};

#endif // PETEST_H