//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "deployconfigtest.h"
#include "deployconfig.h"
#include "targetinfo.h"
#include "distromodule.h"
#include "libinfo.h"

void DeployConfigTest::initTestCase() {
}

void DeployConfigTest::cleanupTestCase() {
}

void DeployConfigTest::targetDirAndIgnoreList() {
    DeployConfig config;

    // No ignore rules are added before a target dir is set.
    LibInfo before;
    before.setName("app.bin");
    before.setPath("/dist");
    before.setPlatform(GeneralFile);
    QVERIFY(config.ignoreList.isIgnore(before) == nullptr);

    config.setTargetDir("/dist");
    QCOMPARE(config.getTargetDir(), QString("/dist"));

    // setTargetDir() adds the target dir to the ignore list,
    // so files inside the target dir are ignored.
    LibInfo inTarget;
    inTarget.setName("app.bin");
    inTarget.setPath("/dist");
    inTarget.setPlatform(GeneralFile);
    QVERIFY(config.ignoreList.isIgnore(inTarget) != nullptr);

    LibInfo outside;
    outside.setName("app.bin");
    outside.setPath("/other");
    outside.setPlatform(GeneralFile);
    QVERIFY(config.ignoreList.isIgnore(outside) == nullptr);
}

void DeployConfigTest::getTargetDir() {
    DeployConfig config;
    config.setTargetDir("/dist");

    TargetInfo app;
    app.setName("app");
    app.setPackage("mypkg");
    config.targetsEdit().insert("app", app);

    QCOMPARE(config.getTargetDir("app"), QString("/dist/mypkg"));
    // An unknown target uses the plain target dir.
    QCOMPARE(config.getTargetDir("unknown"), QString("/dist"));

    DistroModule distro("mypkg");
    config.packagesEdit().insert("mypkg", distro);
    QCOMPARE(config.getPackageTargetDir("mypkg"), QString("/dist/mypkg"));
}

void DeployConfigTest::targetsEdit() {
    DeployConfig config;

    TargetInfo app1;
    app1.setName("app1");
    app1.setPlatform(Unix_x86_64);
    config.targetsEdit().insert("app1", app1);

    TargetInfo app2;
    app2.setName("App2");
    app2.setPlatform(Win32);
    config.targetsEdit().insert("app2", app2);

    QCOMPARE(config.getTargetsListByFilter("APP").size(), 2);
    QCOMPARE(config.getTargetsListByFilter("app1").size(), 1);
    QCOMPARE(config.getTargetsListByFilter("nope").size(), 0);
}

void DeployConfigTest::packagesEdit() {
    DeployConfig config;

    DistroModule module("mypkg");
    module.setName("My Package");
    config.packagesEdit().insert("mypkg", module);

    auto found = config.getDistroFromPackage("mypkg");
    QCOMPARE(found.key(), QString("mypkg"));
    QCOMPARE(found.name(), QString("My Package"));

    // An unknown package returns a module with the requested key
    // and no data at all.
    auto unknown = config.getDistroFromPackage("unknown");
    QCOMPARE(unknown.key(), QString("unknown"));
    QVERIFY(unknown.targets().isEmpty());
}

void DeployConfigTest::getDistro() {
    DeployConfig config;

    DistroModule module("mypkg");
    config.packagesEdit().insert("mypkg", module);

    TargetInfo app;
    app.setName("app");
    app.setPackage("mypkg");
    config.targetsEdit().insert("app", app);

    QCOMPARE(config.getDistro("app").key(), QString("mypkg"));

    TargetInfo noPkg;
    noPkg.setName("noPkg");
    config.targetsEdit().insert("noPkg", noPkg);
    QVERIFY(!config.getDistro("noPkg").isValid());
}

void DeployConfigTest::platform() {
    DeployConfig config;

    DistroModule module("pkg");
    module.addTarget("t32");
    module.addTarget("t64");
    config.packagesEdit().insert("pkg", module);

    TargetInfo t32;
    t32.setName("t32");
    t32.setPlatform(Win32);
    t32.setPackage("pkg");
    config.targetsEdit().insert("t32", t32);

    TargetInfo t64;
    t64.setName("t64");
    t64.setPlatform(Win64);
    t64.setPackage("pkg");
    config.targetsEdit().insert("t64", t64);

    // Win32 | Win64 = Win
    QCOMPARE(config.getPlatform("pkg"), Win);
    // getPlatform() works only for packages, not for target names.
    QCOMPARE(config.getPlatform("t32"), UnknownPlatform);
    QCOMPARE(config.getPlatform("unknown"), UnknownPlatform);
    QCOMPARE(config.getPlatformOfAll(), Win);
}

void DeployConfigTest::defaultPackage() {
    DeployConfig config;
    QVERIFY(config.getDefaultPackage().isEmpty());

    config.setDefaultPackage("pkg");
    QCOMPARE(config.getDefaultPackage(), QString("pkg"));
}

void DeployConfigTest::isNeededQt() {
    DeployConfig config;

    TargetInfo plain;
    plain.setName("plain");
    plain.setPlatform(Unix_x86_64);
    config.targetsEdit().insert("plain", plain);
    QCOMPARE(config.isNeededQt(), NoQt);

    TargetInfo qtApp;
    qtApp.setName("qtApp");
    qtApp.setPath("/opt/apps/qtApp");
    qtApp.setPlatform(Unix_x86_64);
    qtApp.addDependncies("libQt6Core.so");
    config.targetsEdit().insert("qtApp", qtApp);
    QCOMPARE(config.isNeededQt(), Qt6);
}

void DeployConfigTest::customPlatform() {
    DeployConfig config;
    config.setCustomPlatform(Unix_x86_64);
    QCOMPARE(config.customPlatform(), Unix_x86_64);
}

void DeployConfigTest::packagePlatforms() {
    DeployConfig config;

    // The platform of the one package (app bundle) is a simple `platform`
    // property of the DeployConfig object (our collected information about
    // the state of the one package, issue #889): a single Platform bit mask
    // that is empty (UnknownPlatform) by default.
    QCOMPARE(config.platform, UnknownPlatform);

    // The addPlatform() function updates the platform property with the |
    // bit operation.
    LibInfo app;
    app.setName("app");
    app.setPath("/opt/apps/app");
    app.setPlatform(Win64_MinGW_Msvcrt);
    config.addPlatform(app);
    QCOMPARE(config.platform, Win64_MinGW_Msvcrt);

    LibInfo libzip;
    libzip.setName("libzip.dll");
    libzip.setPath("/opt/foreign/libzip.dll");
    libzip.setPlatform(Win64);
    config.addPlatform(libzip);
    QCOMPARE(config.platform,
             static_cast<Platform>(Win64_MinGW_Msvcrt | Win64));

    LibInfo msvcLib;
    msvcLib.setName("msvc.dll");
    msvcLib.setPath("/opt/toolchain/msvc.dll");
    msvcLib.setPlatform(Win64_MSVC);
    config.addPlatform(msvcLib);
    QCOMPARE(config.platform,
             static_cast<Platform>(Win64_MinGW_Msvcrt |
                                   Win64 | Win64_MSVC));

    // The addPlatform() function is idempotent for the platform property:
    // the same library (the same full path) does not change the bit mask a
    // second time.
    config.addPlatform(libzip);
    QCOMPARE(config.platform,
             static_cast<Platform>(Win64_MinGW_Msvcrt |
                                   Win64 | Win64_MSVC));

    // The reset of the whole config clears the platform property.
    config.reset();
    QCOMPARE(config.platform, UnknownPlatform);
}

void DeployConfigTest::reset() {
    DeployConfig config;
    config.setTargetDir("/dist");

    TargetInfo app;
    app.setName("app");
    config.targetsEdit().insert("app", app);

    config.reset();

    QVERIFY(config.getTargetDir().isEmpty());
    QCOMPARE(config.getTargetsListByFilter("app").size(), 0);
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(DeployConfigTest)
