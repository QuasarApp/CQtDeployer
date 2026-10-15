//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "distrostructtest.h"
#include "distrostruct.h"

void DistroStructTest::initTestCase() {
}

void DistroStructTest::cleanupTestCase() {
}

void DistroStructTest::defaults() {
    DistroStruct def;

    // The same defaults are used on all platforms.
    QCOMPARE(def.getRootDir(), QString("/"));
    QCOMPARE(def.getQmlOutDir(), QString("/qml/"));
    QCOMPARE(def.getTrOutDir(), QString("/translations/"));
    QCOMPARE(def.getResOutDir(), QString("/resources/"));
    QCOMPARE(def.getPluginsOutDir(), QString("/plugins/"));
    QCOMPARE(def.getExtraDataOutDir(), QString("/"));

#ifdef Q_OS_LINUX
    // On Linux the lib and bin dirs are separated.
    QCOMPARE(def.getLibOutDir(), QString("/lib/"));
    QCOMPARE(def.getBinOutDir(), QString("/bin/"));
#else
    QCOMPARE(def.getLibOutDir(), QString("/"));
    QCOMPARE(def.getBinOutDir(), QString("/"));
#endif
}

void DistroStructTest::outDirs() {
    DistroStruct s;
    s.setLibOutDir("custom-lib");
    s.setBinOutDir("custom-bin");
    s.setQmlOutDir("custom-qml");
    s.setTrOutDir("custom-tr");
    s.setResOutDir("custom-res");
    s.setPluginsOutDir("custom-plugins");
    s.setExtraDataOutDir("custom-data");

    QCOMPARE(s.getLibOutDir(), QString("/custom-lib/"));
    QCOMPARE(s.getBinOutDir(), QString("/custom-bin/"));
    QCOMPARE(s.getQmlOutDir(), QString("/custom-qml/"));
    QCOMPARE(s.getTrOutDir(), QString("/custom-tr/"));
    QCOMPARE(s.getResOutDir(), QString("/custom-res/"));
    QCOMPARE(s.getPluginsOutDir(), QString("/custom-plugins/"));
    QCOMPARE(s.getExtraDataOutDir(), QString("/custom-data/"));
}

void DistroStructTest::customBasePath() {
    DistroStruct s;
    s.setLibOutDir("lib");
    s.setBinOutDir("bin");

    // The base path is converted to a relative path first,
    // so "/a/b" becomes "/../../" with the current implementation.
    QCOMPARE(s.getRootDir("/a/b"), QString("/../../"));
    QCOMPARE(s.getLibOutDir("/a/b"), QString("/../../lib/"));
    QCOMPARE(s.getBinOutDir("/a/b"), QString("/../../bin/"));
}

void DistroStructTest::empty() {
    DistroStruct s;

    // On all platforms the defaults fill the lib and bin out dirs.
    QVERIFY(!s.isEmpty());

    s.setLibOutDir("");
    s.setBinOutDir("");
    s.setQmlOutDir("");
    s.setTrOutDir("");
    s.setResOutDir("");
    s.setPluginsOutDir("");
    QVERIFY(s.isEmpty());

    // The extra data out dir is not part of the isEmpty() check.
    s.setExtraDataOutDir("data");
    QVERIFY(s.isEmpty());

    s.setLibOutDir("lib");
    QVERIFY(!s.isEmpty());
}
// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(DistroStructTest)
