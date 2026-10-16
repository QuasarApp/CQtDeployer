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
 * Fast unit tests for the LIEF based binary parsing (issue #553).
 *
 * Minimal PE32/PE32+ and Mach-O 64 fixtures are generated at runtime, so
 * no binary fixtures are checked in:
 *  - the PE scanner (PE::getLibInfo) extracts the platform, the import
 *    table modules and the delay import table modules;
 *  - the Mach-O scanner (MachO::getLibInfo) extracts the macOS platform
 *    and the linked dylibs (LC_LOAD_DYLIB);
 *  - the Windows runtime platform detection performed during the PE
 *    parsing stage (issue #855):
 *    a MinGW binary linked against the MSVC CRT imports msvcrt.dll and
 *    gets the *_MinGW_Msvcrt platform; a MinGW binary linked against the
 *    UCRT imports api-ms-win-crt-* modules and gets the *_MinGW_Ucrt
 *    platform; a MSVC toolchain binary imports the MSVC CRT (msvcr*.dll)
 *    and gets the *_MSVC platform; other binaries (including UCRT MSVC
 *    binaries) keep the generic platform.
 */
class PeTest : public QObject
{
    Q_OBJECT
private slots:
    void parseValidFile();
    void parseInvalidFile();
    void parseViaDeployWrapper();
    void parseDelayImports();

    void classifyMinGWFlavors();
    void classifyMSVCFlavor();
    void classifyPrecedenceAndGeneric();
    void recognizeMinGWRuntimes();

    void parseMachO();
    void parseMachORouting();
    void parseInvalidMachO();
};

#endif // PETEST_H