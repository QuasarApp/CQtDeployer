//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "ignoreruletest.h"
#include "ignorerule.h"
#include "libinfo.h"
#include "envirement.h"
#include <QTemporaryDir>

void IgnoreRuleTest::initTestCase() {
}

void IgnoreRuleTest::cleanupTestCase() {
}

static LibInfo makeLib(const QString& name, const QString& path, Platform platform) {
    LibInfo lib;
    lib.setName(name);
    lib.setPath(path);
    lib.setPlatform(platform);
    return lib;
}

static IgnoreData makeData(const QString& label, Platform platform = UnknownPlatform) {
    IgnoreData data(label);
    data.platform = platform;
    return data;
}

void IgnoreRuleTest::label() {
    IgnoreRule rule;
    rule.addRule(makeData("libfoo", Unix_x86_64));

    auto lib = makeLib("libfoo.so", "/opt/lib", Unix_x86_64);
    if (auto match = rule.isIgnore(lib)) {
        QCOMPARE(match->label, QString("libfoo"));
    } else {
        QFAIL("libfoo.so should be ignored by the rule");
    }

    auto other = makeLib("libbar.so", "/opt/lib", Unix_x86_64);
    QVERIFY2(rule.isIgnore(other) == nullptr, "libbar.so should not be ignored");

    // An empty rule list never ignores anything.
    IgnoreRule empty;
    QVERIFY(empty.isIgnore(lib) == nullptr);

    // The case sensitivity of the label depends on the platform.
    IgnoreRule caseRule;
    caseRule.addRule(makeData("LibFoo", Unix_x86_64));

#ifdef Q_OS_WIN
    QVERIFY(caseRule.isIgnore(lib) != nullptr);
#else
    QVERIFY(caseRule.isIgnore(lib) == nullptr);
#endif
}

void IgnoreRuleTest::platform() {
    LibInfo lib = makeLib("libfoo.so", "/opt/lib", Win64);

    // Same platform.
    IgnoreRule same;
    same.addRule(makeData("libfoo", Win64));
    QVERIFY(same.isIgnore(lib) != nullptr);

    // Win64 does not match Win32.
    IgnoreRule win64;
    win64.addRule(makeData("libfoo", Win64));
    lib.setPlatform(Win32);
    QVERIFY(win64.isIgnore(lib) == nullptr);

    // The combined Win flag matches Win32 and Win64.
    IgnoreRule win;
    win.addRule(makeData("libfoo", Win));
    lib.setPlatform(Win32);
    QVERIFY(win.isIgnore(lib) != nullptr);
    lib.setPlatform(Win64);
    QVERIFY(win.isIgnore(lib) != nullptr);

    // Unix_x86_64 does not match Unix_x86_32.
    IgnoreRule unix64;
    unix64.addRule(makeData("libfoo", Unix_x86_64));
    lib.setPlatform(Unix_x86_32);
    QVERIFY(unix64.isIgnore(lib) == nullptr);
    lib.setPlatform(Unix_x86_64);
    QVERIFY(unix64.isIgnore(lib) != nullptr);

    // An unknown platform matches everything.
    IgnoreRule unknown;
    unknown.addRule(makeData("libfoo"));
    lib.setPlatform(Win64);
    QVERIFY(unknown.isIgnore(lib) != nullptr);

    // General files are not matched by platform rules.
    lib.setPlatform(GeneralFile);
    QVERIFY(win64.isIgnore(lib) == nullptr);
    QVERIFY(unknown.isIgnore(lib) != nullptr);
}

void IgnoreRuleTest::priority() {
    LibInfo lib = makeLib("libfoo.so", "/opt/lib", Unix_x86_64);

    // NotFile matches every priority.
    IgnoreRule notFile;
    notFile.addRule(makeData("libfoo", Unix_x86_64));
    lib.setPriority(QtLib);
    QVERIFY(notFile.isIgnore(lib) != nullptr);

    // SystemLib matches SystemLib and everything above it.
    IgnoreRule systemLib;
    auto systemData = makeData("libfoo", Unix_x86_64);
    systemData.prority = SystemLib;
    systemLib.addRule(systemData);
    lib.setPriority(SystemLib);
    QVERIFY(systemLib.isIgnore(lib) != nullptr);
    lib.setPriority(AlienLib);
    QVERIFY(systemLib.isIgnore(lib) != nullptr);

    // SystemLib does not match QtLib.
    lib.setPriority(QtLib);
    QVERIFY(systemLib.isIgnore(lib) == nullptr);

    // QtLib matches QtLib.
    IgnoreRule qtLib;
    auto qtData = makeData("libfoo", Unix_x86_64);
    qtData.prority = QtLib;
    qtLib.addRule(qtData);
    lib.setPriority(QtLib);
    QVERIFY(qtLib.isIgnore(lib) != nullptr);
}

void IgnoreRuleTest::environmentRule() {
    QTemporaryDir dirA;
    QTemporaryDir dirB;
    QVERIFY2(dirA.isValid() && dirB.isValid(), "create temporary dirs");

    const QString fileA = dirA.path() + "/libinA.so";
    const QString fileB = dirB.path() + "/libinA.so";
    for (const auto& file : {fileA, fileB}) {
        QFile f(file);
        QVERIFY2(f.open(QIODevice::WriteOnly), qPrintable("create " + file));
        f.write("lib");
        f.close();
    }

    IgnoreRule rule;
    auto data = makeData("libinA", GeneralFile);
    data.enfirement.addEnv(dirA.path());
    rule.addRule(data);

    auto libA = makeLib("libinA.so", dirA.path(), GeneralFile);
    QVERIFY(rule.isIgnore(libA) != nullptr);

    auto libB = makeLib("libinA.so", dirB.path(), GeneralFile);
    QVERIFY(rule.isIgnore(libB) == nullptr);
}


void IgnoreRuleTest::windowsRuntimeFlavors() {
    // The Windows runtime flavors (issue #855): a generic Windows platform
    // rule must match all runtime flavors, a flavored rule only the
    // identical runtime flavor.

    LibInfo msvcrtLib;
    msvcrtLib.setName("libstdc++-6.dll");
    msvcrtLib.setPath("/opt/mingw/bin");
    msvcrtLib.setPlatform(Platform::Win64_MinGW_Msvcrt);

    LibInfo ucrtLib = msvcrtLib;
    ucrtLib.setPlatform(Platform::Win64_MinGW_Ucrt);

    IgnoreData win64Rule("libstdc++");
    win64Rule.platform = Platform::Win64;
    IgnoreRule win64Rules;
    win64Rules.addRule(win64Rule);
    // A generic Windows rule must match both MinGW flavors
    QVERIFY2(win64Rules.isIgnore(msvcrtLib) != nullptr,
             "generic Win64 rule must match MSVCRT MinGW lib");
    QVERIFY2(win64Rules.isIgnore(ucrtLib) != nullptr,
             "generic Win64 rule must match UCRT MinGW lib");

    IgnoreData msvcrtRule("libstdc++");
    msvcrtRule.platform = Platform::Win64_MinGW_Msvcrt;
    IgnoreRule msvcrtRules;
    msvcrtRules.addRule(msvcrtRule);
    // A flavored rule must only match the identical flavor
    QVERIFY2(msvcrtRules.isIgnore(msvcrtLib) != nullptr,
             "MSVCRT rule must match MSVCRT MinGW lib");
    QVERIFY2(msvcrtRules.isIgnore(ucrtLib) == nullptr,
             "MSVCRT rule must NOT match UCRT MinGW lib");

    IgnoreData ucrtRule("libstdc++");
    ucrtRule.platform = Platform::Win64_MinGW_Ucrt;
    IgnoreRule ucrtRules;
    ucrtRules.addRule(ucrtRule);
    QVERIFY2(ucrtRules.isIgnore(ucrtLib) != nullptr,
             "UCRT rule must match UCRT MinGW lib");
    QVERIFY2(ucrtRules.isIgnore(msvcrtLib) == nullptr,
             "UCRT rule must NOT match MSVCRT MinGW lib");

    // A MSVC flavored library (same name, so the rule name matching
    // does not hide the platform check)
    LibInfo msvcLib = msvcrtLib;
    msvcLib.setPlatform(Platform::Win64_MSVC);

    // A generic Windows rule must match the MSVC flavor as well
    QVERIFY2(win64Rules.isIgnore(msvcLib) != nullptr,
             "generic Win64 rule must match MSVC lib");

    IgnoreData msvcRule("libstdc++");
    msvcRule.platform = Platform::Win64_MSVC;
    IgnoreRule msvcRules;
    msvcRules.addRule(msvcRule);
    // A flavored rule must only match the identical flavor
    QVERIFY2(msvcRules.isIgnore(msvcLib) != nullptr,
             "MSVC rule must match MSVC lib");
    QVERIFY2(msvcRules.isIgnore(msvcrtLib) == nullptr,
             "MSVC rule must NOT match MSVCRT MinGW lib");
    QVERIFY2(msvcRules.isIgnore(ucrtLib) == nullptr,
             "MSVC rule must NOT match UCRT MinGW lib");
    // MinGW flavored rules must not match the MSVC lib
    QVERIFY2(msvcrtRules.isIgnore(msvcLib) == nullptr,
             "MSVCRT MinGW rule must NOT match MSVC lib");
    QVERIFY2(ucrtRules.isIgnore(msvcLib) == nullptr,
             "UCRT MinGW rule must NOT match MSVC lib");
}


// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(IgnoreRuleTest)
