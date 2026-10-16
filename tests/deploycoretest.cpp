//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "deploycoretest.h"
#include "deploycore.h"

void DeployCoreTest::initTestCase() {
}

void DeployCoreTest::cleanupTestCase() {
}

void DeployCoreTest::platformCompatibility() {
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Msvcrt,
                                              Platform::Win64_MinGW_Msvcrt), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Ucrt,
                                              Platform::Win64_MinGW_Ucrt), true);
    // MSVCRT and UCRT MinGW runtime sets must never be mixed
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Ucrt,
                                              Platform::Win64_MinGW_Msvcrt), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Msvcrt,
                                              Platform::Win64_MinGW_Ucrt), false);
    // Generic (non flavored) platforms stay compatible with the same bitness
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Msvcrt,
                                              Platform::Win64), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64,
                                              Platform::Win64_MinGW_Msvcrt), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64,
                                              Platform::Win64_MinGW_Ucrt), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64,
                                              Platform::Win64), true);
    // The MSVC family matches the generic platform of the same bitness
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MSVC,
                                              Platform::Win64_MSVC), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MSVC,
                                              Platform::Win64), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64,
                                              Platform::Win64_MSVC), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win32_MSVC,
                                              Platform::Win32), true);
    // MSVC and MinGW runtime sets must never be mixed
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Msvcrt,
                                              Platform::Win64_MSVC), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MSVC,
                                              Platform::Win64_MinGW_Msvcrt), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Ucrt,
                                              Platform::Win64_MSVC), false);
    // Different bitness or architecture stays incompatible
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win32_MinGW_Msvcrt,
                                              Platform::Win64_MinGW_Msvcrt), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Msvcrt,
                                              Platform::Win_ARM_64), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64,
                                              Platform::Win32), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win32_MSVC,
                                              Platform::Win64_MSVC), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MSVC,
                                              Platform::Win_ARM_64), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::UnknownPlatform,
                                              Platform::Win64), false);
}

void DeployCoreTest::platformStrings() {
    QCOMPARE(DeployCore::platformToString(Platform::Win64_MinGW_Msvcrt),
             QStringLiteral("win_x86_64, MinGW_Msvcrt"));
    QCOMPARE(DeployCore::platformToString(Platform::Win32_MinGW_Ucrt),
             QStringLiteral("win_x86, MinGW_Ucrt"));
    QCOMPARE(DeployCore::platformToString(Platform::Win64_MSVC),
             QStringLiteral("win_x86_64, MSVC"));
    QCOMPARE(DeployCore::platformToString(Platform::Win32_MSVC),
             QStringLiteral("win_x86, MSVC"));
    QCOMPARE(DeployCore::platformToString(Platform::Win64),
             QStringLiteral("win_x86_64"));
    QCOMPARE(DeployCore::getPlatformFromString("win_x86_64_msvcrt"),
             Platform::Win64_MinGW_Msvcrt);
    QCOMPARE(DeployCore::getPlatformFromString("win_x86_ucrt"),
             Platform::Win32_MinGW_Ucrt);
    QCOMPARE(DeployCore::getPlatformFromString("win_x86_64_msvc"),
             Platform::Win64_MSVC);
    QCOMPARE(DeployCore::getPlatformFromString("win_x86_msvc"),
             Platform::Win32_MSVC);
    QCOMPARE(DeployCore::getPlatformFromString("win_x86_64"),
             Platform::Win64);
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(DeployCoreTest)