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

void DeployCoreTest::symbolSplitting() {
    QString name;
    QString version;

    // Unversioned symbol: the whole string is the name
    DeployCore::splitSymbolVersion("printf", name, version);
    QCOMPARE(name, QString("printf"));
    QVERIFY(version.isEmpty());

    // Versioned symbol: name and version are separated at the first '@'
    DeployCore::splitSymbolVersion("memcpy@GLIBC_2.2.5", name, version);
    QCOMPARE(name, QString("memcpy"));
    QCOMPARE(version, QString("GLIBC_2.2.5"));

    // A symbol name containing '@' splits at the first separator
    DeployCore::splitSymbolVersion("operator@@new", name, version);
    QCOMPARE(name, QString("operator"));
    QCOMPARE(version, QString("@new"));

    // A leading '@' is not a version separator, the string stays a name
    DeployCore::splitSymbolVersion("@GLIBC_2.2.5", name, version);
    QCOMPARE(name, QString("@GLIBC_2.2.5"));
    QVERIFY(version.isEmpty());
}

void DeployCoreTest::symbolVersionCompatibility() {
    // Identical versions always match
    QCOMPARE(DeployCore::symbolVersionCompatible("GLIBCXX_3.4.31",
                                                 "GLIBCXX_3.4.31"), true);
    // A definition at a coarser version node of the same family (the
    // required numbers start with the defined numbers) is accepted
    QCOMPARE(DeployCore::symbolVersionCompatible("GLIBCXX_3.4.31",
                                                 "GLIBCXX_3.4"), true);
    QCOMPARE(DeployCore::symbolVersionCompatible("GLIBCXX_3.4",
                                                 "GLIBCXX_3.4"), true);
    // Different nodes of the same family are not interchangeable
    QCOMPARE(DeployCore::symbolVersionCompatible("GLIBCXX_3.4.31",
                                                 "GLIBCXX_3.4.29"), false);
    QCOMPARE(DeployCore::symbolVersionCompatible("GLIBC_2.2.5",
                                                 "GLIBC_2.14"), false);
    // A definition more specific than the requirement is not accepted
    QCOMPARE(DeployCore::symbolVersionCompatible("GLIBCXX_3.4",
                                                 "GLIBCXX_3.4.31"), false);
    // Different version families never match
    QCOMPARE(DeployCore::symbolVersionCompatible("GLIBCXX_3.4.31",
                                                 "GLIBC_2.2.5"), false);
    QCOMPARE(DeployCore::symbolVersionCompatible("GLIBC_2.2.5",
                                                 "GLIBCXX_3.4.31"), false);
    // Versions without numbers only match exactly
    QCOMPARE(DeployCore::symbolVersionCompatible("CXXABIV1", "CXXABIV1"),
             true);
    QCOMPARE(DeployCore::symbolVersionCompatible("CXXABIV1", "CXXABI_1.3"),
             false);
    // An empty required version (unversioned requirement) always matches
    QCOMPARE(DeployCore::symbolVersionCompatible("", "GLIBC_2.2.5"), true);
}

void DeployCoreTest::symbolProvides() {
    const QSet<QString> providedSymbols = {
        "printf",                 // unversioned definition
        "memcpy@GLIBC_2.2.5",     // versioned definition
        "myexport@MYLIB_2.0",     // versioned definition
        "unique@CXXABIV1",        // non numeric version
        "prefixsym@GLIBCXX_3.4",  // defined at a coarse version node
        "othersym@GLIBCXX_3.4.31" // defines the fine version node
    };
    const QSet<QString> providedVersions = {"GLIBC_2.2.5", "MYLIB_2.0",
                                            "CXXABIV1", "GLIBCXX_3.4",
                                            "GLIBCXX_3.4.31"};

    // Unversioned requirement: satisfied by any definition of the name
    QVERIFY(DeployCore::providesSymbol(providedSymbols, providedVersions,
                                       "printf"));
    QVERIFY(DeployCore::providesSymbol(providedSymbols, providedVersions,
                                       "memcpy"));
    // Unknown symbol
    QVERIFY(!DeployCore::providesSymbol(providedSymbols, providedVersions,
                                        "qDebug"));

    // Exact version match
    QVERIFY(DeployCore::providesSymbol(providedSymbols, providedVersions,
                                       "memcpy@GLIBC_2.2.5"));
    // Same name but a version node the candidate does not define
    QVERIFY(!DeployCore::providesSymbol(providedSymbols, providedVersions,
                                        "myexport@MYLIB_9.9"));
    // Same name, required version node not defined by the candidate
    QVERIFY(!DeployCore::providesSymbol(providedSymbols, providedVersions,
                                        "memcpy@GLIBC_2.39"));
    // A requirement at a fine node of the same family is satisfied by a
    // symbol defined at a coarser node, if the node exists
    QVERIFY(DeployCore::providesSymbol(providedSymbols, providedVersions,
                                       "prefixsym@GLIBCXX_3.4.31"));
    // ...but not for a node the candidate does not define
    QVERIFY(!DeployCore::providesSymbol(providedSymbols, providedVersions,
                                        "prefixsym@GLIBCXX_3.5.0"));
    // Non numeric versions only match exactly
    QVERIFY(DeployCore::providesSymbol(providedSymbols, providedVersions,
                                       "unique@CXXABIV1"));
    QVERIFY(!DeployCore::providesSymbol(providedSymbols, providedVersions,
                                        "unique@CXXABI_1.3"));
}

void DeployCoreTest::symbolMatchScore() {
    const QSet<QString> providedSymbols = {
        "printf",
        "memcpy@GLIBC_2.2.5",
        "qDebug@Qt_6",
    };
    const QSet<QString> providedVersions = {"GLIBC_2.2.5", "Qt_6"};

    // Empty requirement set
    QCOMPARE(DeployCore::symbolMatchScore({}, providedSymbols,
                                          providedVersions), 0);
    // A provider without symbols never matches
    QCOMPARE(DeployCore::symbolMatchScore({"printf"}, {}, {}), 0);

    const QSet<QString> required = {"printf", "memcpy@GLIBC_2.2.5",
                                    "qDebug@Qt_6", "missing"};
    // Three of the four required symbols are provided
    QCOMPARE(DeployCore::symbolMatchScore(required, providedSymbols,
                                          providedVersions), 3);

    // A versioned requirement that cannot be satisfied is not counted
    const QSet<QString> requiredNewer = {"memcpy@GLIBC_2.39"};
    QCOMPARE(DeployCore::symbolMatchScore(requiredNewer, providedSymbols,
                                          providedVersions), 0);
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(DeployCoreTest)