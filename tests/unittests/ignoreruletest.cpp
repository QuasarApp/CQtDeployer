//
// Copyright (C) 2026 QuasarApp.
// Distributed under the GPLv3 software license, see the accompanying
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

