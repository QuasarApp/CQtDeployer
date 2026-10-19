//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "deploycoretest.h"
#include "deploycore.h"
#include "deployconfig.h"
#include "libinfo.h"

void DeployCoreTest::initTestCase() {
}

void DeployCoreTest::cleanupTestCase() {
}

void DeployCoreTest::platformCompatibility() {
    // The platform comparison is a strong (exact) comparison: a candidate
    // library satisfies a dependency only when it is built for exactly the
    // same platform as the requesting library (issue #870).
    //
    // Identical platforms are always compatible.
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Msvcrt,
                                              Platform::Win64_MinGW_Msvcrt), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Ucrt,
                                              Platform::Win64_MinGW_Ucrt), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64,
                                              Platform::Win64), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MSVC,
                                              Platform::Win64_MSVC), true);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win32_MSVC,
                                              Platform::Win32_MSVC), true);
    // MSVCRT and UCRT MinGW runtime sets must never be mixed
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Ucrt,
                                              Platform::Win64_MinGW_Msvcrt), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Msvcrt,
                                              Platform::Win64_MinGW_Ucrt), false);
    // A generic (non flavored) platform is NOT a valid substitute for a
    // flavored one, and vice versa. This is the regression from issue #870:
    // matching a generic Windows platform against a flavored one allowed the
    // scanner to pick a foreign runtime set (Strawberry Perl) instead of the
    // target toolchain (Qt MinGW).
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MinGW_Msvcrt,
                                              Platform::Win64), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64,
                                              Platform::Win64_MinGW_Msvcrt), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64,
                                              Platform::Win64_MinGW_Ucrt), false);
    // The MSVC family does not match the generic platform of the same bitness
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64_MSVC,
                                              Platform::Win64), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win64,
                                              Platform::Win64_MSVC), false);
    QCOMPARE(DeployCore::isPlatformCompatible(Platform::Win32_MSVC,
                                              Platform::Win32), false);
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

void DeployCoreTest::foreignRuntimeNotSelected() {
    // Reproduces the selection scenario of issue #870: a target application
    // built with the Qt MinGW UCRT toolchain has two candidate libraries for
    // one of its runtime dependencies in the deploy environment:
    //   * the library shipped with the Qt MinGW toolchain (same platform),
    //   * a foreign library (e.g. the MinGW runtime shipped by Strawberry
    //     Perl) that is only a generic Win64 library.
    //
    // The dependency scanner filters the candidates with
    // DeployCore::isPlatformCompatible(candidate, wanted) where `wanted` is
    // the platform of the parent library (the application) and `candidate` is
    // the platform of each candidate. With the strong platform comparison the
    // only compatible candidate is the one from the Qt toolchain, so the
    // foreign library is never selected.
    DeployConfig config;
    DeployCore::_config = &config;

    const Platform targetPlatform = Platform::Win64_MinGW_Ucrt; // the app

    // Candidate built by the target (Qt MinGW UCRT) toolchain.
    LibInfo qtToolchainLib;
    qtToolchainLib.setName("libstdc++-6.dll");
    qtToolchainLib.setPath("C:/Qt/6.10.3/mingw_64/bin/libstdc++-6.dll");
    qtToolchainLib.setPlatform(Platform::Win64_MinGW_Ucrt);

    // Foreign candidate from a third party distribution (Strawberry Perl)
    // whose MinGW runtime is detected as a generic Win64 library.
    LibInfo perlLib;
    perlLib.setName("libstdc++-6.dll");
    perlLib.setPath("C:/STRAWBERRY/PERL/BIN/libstdc++-6.dll");
    perlLib.setPlatform(Platform::Win64);

    // The Qt toolchain library is compatible with the target application.
    QCOMPARE(DeployCore::isPlatformCompatible(qtToolchainLib.getPlatform(),
                                              targetPlatform), true);
    // The Strawberry Perl library must NOT be selected for the target.
    QCOMPARE(DeployCore::isPlatformCompatible(perlLib.getPlatform(),
                                              targetPlatform), false);

    // A foreign library with a different MinGW flavor (Msvcrt) is rejected as
    // well.
    LibInfo perlMsvcrtLib;
    perlMsvcrtLib.setName("libgcc_s_seh-1.dll");
    perlMsvcrtLib.setPath("C:/STRAWBERRY/PERL/BIN/libgcc_s_seh-1.dll");
    perlMsvcrtLib.setPlatform(Platform::Win64_MinGW_Msvcrt);
    QCOMPARE(DeployCore::isPlatformCompatible(perlMsvcrtLib.getPlatform(),
                                              targetPlatform), false);

    DeployCore::_config = nullptr;
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