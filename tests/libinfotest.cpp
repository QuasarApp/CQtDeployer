//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "libinfotest.h"
#include "libinfo.h"
#include <QHash>

void LibInfoTest::initTestCase() {
}

void LibInfoTest::cleanupTestCase() {
}

void LibInfoTest::defaultValues() {
    LibInfo lib;
    QVERIFY(lib.getName().isEmpty());
    QVERIFY(lib.getPath().isEmpty());
    QCOMPARE(lib.getPlatform(), UnknownPlatform);
    QCOMPARE(lib.getPriority(), NotFile);
    QCOMPARE(lib.getWinApi(), WinAPI::NoWinAPI);
    QCOMPARE(lib.fullPath(), QString("/"));
    QVERIFY(!lib.isValid());
    QVERIFY(!lib.isScaned());
    QVERIFY(lib.getDependncies().isEmpty());
}

void LibInfoTest::namePathAndFullpath() {
    LibInfo lib;
    lib.setName("libfoo.so");
    lib.setPath("/opt/lib");
    QCOMPARE(lib.getName(), QString("libfoo.so"));
    QCOMPARE(lib.getPath(), QString("/opt/lib"));
    QCOMPARE(lib.fullPath(), QString("/opt/lib/libfoo.so"));
}

void LibInfoTest::isValid() {
    LibInfo lib;
    QVERIFY(!lib.isValid());

    lib.setName("libfoo.so");
    lib.setPath("/opt/lib");
    QVERIFY(!lib.isValid());

    lib.setPlatform(Unix_x86_64);
    QVERIFY(lib.isValid());
}

void LibInfoTest::platform() {
    LibInfo lib;
    lib.setPlatform(Win64);
    QCOMPARE(lib.getPlatform(), Win64);
    lib.setPlatform(Unix_x86_32);
    QCOMPARE(lib.getPlatform(), Unix_x86_32);
}

void LibInfoTest::dependencies() {
    LibInfo lib;
    lib.addDependncies("liba.so");
    lib.addDependncies({"libb.so", "libc.so"});
    lib.addDepends("libd.so");

    auto deps = lib.getDependncies();
    QCOMPARE(deps.size(), 4);
    QVERIFY(deps.contains("liba.so"));
    QVERIFY(deps.contains("libb.so"));
    QVERIFY(deps.contains("libc.so"));
    QVERIFY(deps.contains("libd.so"));

    lib.removeDependncies("liba.so");
    QVERIFY(!lib.getDependncies().contains("liba.so"));
    QCOMPARE(lib.getDependncies().size(), 3);

    lib.setDependncies({"libx.so"});
    QCOMPARE(lib.getDependncies(), QSet<QString>{"libx.so"});
}

void LibInfoTest::isDependetOfQt() {
    LibInfo lib;
    QCOMPARE(lib.isDependetOfQt(), NoQt);

    lib.addDependncies("libQt6Core.so");
    QCOMPARE(lib.isDependetOfQt(), Qt6);

    LibInfo qt5;
    qt5.addDependncies("libQt5Gui.so");
    QCOMPARE(qt5.isDependetOfQt(), Qt5);

    LibInfo qt4;
    qt4.addDependncies("Qt4Test.dll");
    QCOMPARE(qt4.isDependetOfQt(), Qt4);

    LibInfo plain;
    plain.addDependncies("libfoo.so");
    QCOMPARE(plain.isDependetOfQt(), NoQt);
}

void LibInfoTest::priority() {
    LibInfo lib;
    lib.setPriority(SystemLib);
    QCOMPARE(lib.getPriority(), SystemLib);
}

void LibInfoTest::rpath() {
    LibInfo lib;
    lib.setRPath("$ORIGIN/../lib");
    QCOMPARE(lib.getRPath(), QString("$ORIGIN/../lib"));
}

void LibInfoTest::winApi() {
    LibInfo lib;
    lib.setWinApi(WinAPI::Core);
    QCOMPARE(lib.getWinApi(), WinAPI::Core);
}

void LibInfoTest::clear() {
    LibInfo lib;
    lib.setName("libfoo.so");
    lib.setPath("/opt/lib");
    lib.setPlatform(Win64);
    lib.setPriority(SystemLib);
    lib.setRPath("$ORIGIN");
    lib.setWinApi(WinAPI::Core);
    lib.addDependncies("liba.so");

    // clear() resets name, path, rpath, platform and the dependencies.
    // It does not touch the priority and the WinAPI state
    // (implementation behavior).
    lib.clear();

    QVERIFY(lib.getName().isEmpty());
    QVERIFY(lib.getPath().isEmpty());
    QCOMPARE(lib.getPlatform(), UnknownPlatform);
    QCOMPARE(lib.getPriority(), SystemLib);
    QCOMPARE(lib.getWinApi(), WinAPI::Core);
    QVERIFY(lib.getRPath().isEmpty());
    QVERIFY(lib.getDependncies().isEmpty());
    QVERIFY(!lib.isValid());
}

void LibInfoTest::compare() {
    LibInfo a;
    a.setName("libfoo.so");
    a.setPath("/opt/lib");

    LibInfo b;
    b.setName("libfoo.so");
    b.setPath("/opt/lib");

    LibInfo c;
    c.setName("libbar.so");
    c.setPath("/opt/lib");

    QVERIFY(a == b);
    QVERIFY(!(a == c));
    QCOMPARE(qHash(a), qHash(b));
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(LibInfoTest)
