//#
//# Copyright (C) 2020-2026 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#

#ifndef PEPATFORMTEST_H
#define PEPATFORMTEST_H

#include "testbase.h"
#include "testutils.h"

#include <QtTest>

/**
 * @brief The PePlatformTest class
 * Checks the Windows runtime platform detection during PE parsing (issue #855).
 *
 * Minimal PE fixtures are generated at runtime:
 *  - a MinGW binary linked against the MSVC CRT imports msvcrt.dll and
 *    gets the *_MinGW_Msvcrt platform;
 *  - a MinGW binary linked against the UCRT imports api-ms-win-crt-*
 *    modules and gets the *_MinGW_Ucrt platform;
 *  - a MSVC toolchain binary imports the MSVC CRT (msvcr*.dll) and gets
 *    the *_MSVC platform;
 *  - other binaries (including UCRT MSVC binaries) keep the generic
 *    platform.
 */
class PePlatformTest: public TestBase, protected TestUtils
{
public:
    PePlatformTest();
    ~PePlatformTest();

    void test();

private:
    static QByteArray buildTestPe(const QStringList &dllNames, bool pe64);

    void scanPlatform(const QTemporaryDir &dir,
                      const QString &fileName,
                      const QStringList &dllNames,
                      bool pe64,
                      Platform &platform);
};

#endif // PEPATFORMTEST_H