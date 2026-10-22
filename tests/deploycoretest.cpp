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

#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <QFileInfo>

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

void DeployCoreTest::genericWindowsFallback() {
    // The generic Windows fallback (issue #889) is used only for a
    // flavored parent and only for the generic platform of the same base
    // bitness, so a plain C dependency (for example libzip.dll, classified
    // as a generic Win64 library) is not silently dropped by the strong
    // platform comparison. A foreign flavored runtime is never a valid
    // fallback (issue #870).
    //
    // Generic candidate for a flavored parent of the same bitness.
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win64,
                                                  Platform::Win64_MinGW_Msvcrt),
             true);
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win64,
                                                  Platform::Win64_MinGW_Ucrt),
             true);
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win64,
                                                  Platform::Win64_MSVC),
             true);
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win32,
                                                  Platform::Win32_MinGW_Msvcrt),
             true);
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win32,
                                                  Platform::Win32_MinGW_Ucrt),
             true);
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win32,
                                                  Platform::Win32_MSVC),
             true);
    // Different base bitness is never a fallback.
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win32,
                                                  Platform::Win64_MinGW_Msvcrt),
             false);
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win64,
                                                  Platform::Win32_MinGW_Ucrt),
             false);
    // A generic candidate of the same base platform is a valid fallback
    // also for the (currently unused) flavored ARM platforms.
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win_ARM_64,
                                                  static_cast<Platform>(
                                                      Platform::Win_ARM_64 |
                                                      Platform::MinGW_Msvcrt)),
             true);
    // A flavored candidate is never a "generic" fallback.
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win64_MinGW_Msvcrt,
                                                  Platform::Win64_MinGW_Ucrt),
             false);
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win64_MSVC,
                                                  Platform::Win64_MinGW_Msvcrt),
             false);
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win64_MinGW_Msvcrt,
                                                  Platform::Win64_MinGW_Msvcrt),
             false);
    // A generic (non flavored) parent never uses the fallback: its
    // candidates are selected by the exact comparison only.
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win64_MinGW_Msvcrt,
                                                  Platform::Win64),
             false);
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Win64,
                                                  Platform::Win64),
             false);
    // Non-Windows platforms never use the fallback.
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Unix_x86_64,
                                                  Platform::Unix_x86_64),
             false);
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::Mac_ARM_64,
                                                  Platform::Mac_X86_64),
             false);
    QCOMPARE(DeployCore::isGenericWindowsFallback(Platform::UnknownPlatform,
                                                  Platform::Win64_MinGW_Msvcrt),
             false);
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

void DeployCoreTest::qtRootDirQmakeDetection() {
    // The qtRootDir option (issue #788): CQtDeployer detects the
    // qmake/qmake6 executable from the bin directory of the Qt
    // installation root and uses it to parse the Qt module locations.

    QTemporaryDir tmp;
    QVERIFY(tmp.isValid());

    auto makeExec = [](const QString &path) {
        auto file = QFile(path);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.close();
        QVERIFY(QFile::setPermissions(path,
                                      QFileInfo(path).permissions()
                                      | QFileDevice::ExeOwner
                                      | QFileDevice::ExeGroup
                                      | QFileDevice::ExeOther));
    };

    // Qt 5 layout: <root>/bin/qmake
    const auto qt5Root = tmp.path() + "/qt5";
    QVERIFY(QDir(qt5Root + "/bin").mkpath(qt5Root + "/bin"));
    const auto qt5Qmake = qt5Root + "/bin/qmake";
    makeExec(qt5Qmake);
    QCOMPARE(DeployCore::findQmakeInQtRoot(qt5Root), qt5Qmake);

    // Qt 6 layout: <root>/bin/qmake6 (no qmake)
    const auto qt6Root = tmp.path() + "/qt6";
    QVERIFY(QDir(qt6Root + "/bin").mkpath(qt6Root + "/bin"));
    const auto qt6Qmake = qt6Root + "/bin/qmake6";
    makeExec(qt6Qmake);
    QCOMPARE(DeployCore::findQmakeInQtRoot(qt6Root), qt6Qmake);

    // When both executables exist the qmake is preferred
    const auto bothRoot = tmp.path() + "/both";
    QVERIFY(QDir(bothRoot + "/bin").mkpath(bothRoot + "/bin"));
    const auto bothQmake = bothRoot + "/bin/qmake";
    const auto bothQmake6 = bothRoot + "/bin/qmake6";
    makeExec(bothQmake);
    makeExec(bothQmake6);
    QCOMPARE(DeployCore::findQmakeInQtRoot(bothRoot), bothQmake);

    // A passed bin directory (without the bin sub-directory) is found too
    QCOMPARE(DeployCore::findQmakeInQtRoot(qt5Root + "/bin"), qt5Qmake);

    // A non-existing directory returns an empty path
    QCOMPARE(DeployCore::findQmakeInQtRoot(tmp.path() + "/no-such-dir"),
             QString());

    // A root without a bin directory and without qmake returns empty
    const auto emptyRoot = tmp.path() + "/empty";
    QVERIFY(QDir(emptyRoot).mkpath(emptyRoot));
    QCOMPARE(DeployCore::findQmakeInQtRoot(emptyRoot), QString());

    // A root with an empty bin directory returns empty
    const auto noQmakeRoot = tmp.path() + "/noqmake";
    QVERIFY(QDir(noQmakeRoot + "/bin").mkpath(noQmakeRoot + "/bin"));
    QCOMPARE(DeployCore::findQmakeInQtRoot(noQmakeRoot), QString());
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(DeployCoreTest)