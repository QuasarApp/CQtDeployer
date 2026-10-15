//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "distromoduletest.h"
#include "distromodule.h"

void DistroModuleTest::initTestCase() {
}

void DistroModuleTest::cleanupTestCase() {
}

void DistroModuleTest::key() {
    // The key is set by the constructor, setKey() is protected.
    DistroModule module("mypkg");
    QCOMPARE(module.key(), QString("mypkg"));
}

void DistroModuleTest::isValid() {
    DistroModule valid("pkg");
    QVERIFY(valid.isValid());

    DistroModule invalid("");
    QVERIFY(!invalid.isValid());
}

void DistroModuleTest::targets() {
    DistroModule module("pkg");
    QVERIFY(module.targets().isEmpty());

    module.addTarget("t1");
    module.addTarget("t2");
    QCOMPARE(module.targets(), (QSet<QString>{"t1", "t2"}));

    module.setTargets(QSet<QString>{"t3"});
    QCOMPARE(module.targets(), QSet<QString>{"t3"});
}

void DistroModuleTest::qmlInput() {
    DistroModule module("pkg");
    QVERIFY(module.qmlInput().isEmpty());

    module.addQmlInput("main.qml");
    QCOMPARE(module.qmlInput(), QSet<QString>{"main.qml"});

    module.setQmlInput(QSet<QString>{"other.qml"});
    QCOMPARE(module.qmlInput(), QSet<QString>{"other.qml"});
}

void DistroModuleTest::metaData() {
    DistroModule module("pkg");
    QVERIFY(module.name().isEmpty());

    module.setName("My Package");
    module.setDescription("A test package");
    module.setVersion("1.0.0");
    module.setReleaseData("2026-01-01");
    module.setPublisher("QuasarApp");
    module.setHomePage("https://quasarapp.com");
    module.setPrefix("myapp");

    QCOMPARE(module.name(), QString("My Package"));
    QCOMPARE(module.description(), QString("A test package"));
    QCOMPARE(module.version(), QString("1.0.0"));
    QCOMPARE(module.releaseData(), QString("2026-01-01"));
    QCOMPARE(module.publisher(), QString("QuasarApp"));
    QCOMPARE(module.homePage(), QString("https://quasarapp.com"));
    QCOMPARE(module.prefix(), QString("myapp"));
}

void DistroModuleTest::plugins() {
    DistroModule module("pkg");
    QVERIFY(module.enabledPlugins().isEmpty());
    QVERIFY(module.disabledPlugins().isEmpty());
    QVERIFY(module.extraPlugins().isEmpty());

    module.addEnabledPlugins("plugins/a");
    module.setDisabledPlugins(QSet<QString>{"plugins/b"});
    module.addExtraPlugins("plugins/c");

    QCOMPARE(module.enabledPlugins(), QSet<QString>{"plugins/a"});
    QCOMPARE(module.disabledPlugins(), QSet<QString>{"plugins/b"});
    QCOMPARE(module.extraPlugins(), QSet<QString>{"plugins/c"});
}

void DistroModuleTest::extraData() {
    DistroModule module("pkg");
    QVERIFY(module.extraData().isEmpty());

    module.addExtraData("data/file1.txt");
    module.addExtraData("data/file2.txt");
    QCOMPARE(module.extraData(),
             (QSet<QString>{"data/file1.txt", "data/file2.txt"}));

    module.setExtraData(QSet<QString>{});
    QVERIFY(module.extraData().isEmpty());
}

void DistroModuleTest::translation() {
    DistroModule module("pkg");
    QVERIFY(module.translation().isEmpty());

    module.addTranslation("en.qm");
    module.addTranslation("ru.qm");
    QCOMPARE(module.translation(), (QSet<QString>{"en.qm", "ru.qm"}));

    module.setTranslation(QSet<QString>{"de.qm"});
    QCOMPARE(module.translation(), QSet<QString>{"de.qm"});
}

void DistroModuleTest::outputs() {
    DistroModule module("pkg");
    QCOMPARE(module.installDirDEB(), QString("/opt"));
    QVERIFY(module.debOut().isEmpty());
    QVERIFY(module.zipOut().isEmpty());

    module.setInstallDirDEB("/usr");
    module.setDebOut("/opt/deb/pkg.deb");
    module.setZipOut("/opt/zip/pkg.zip");

    QCOMPARE(module.installDirDEB(), QString("/usr"));
    QCOMPARE(module.debOut(), QString("/opt/deb/pkg.deb"));
    QCOMPARE(module.zipOut(), QString("/opt/zip/pkg.zip"));
}

void DistroModuleTest::copy() {
    DistroModule module("pkg");
    module.setName("My Package");
    module.setVersion("1.0");
    module.addTarget("t1");

    DistroModule copy(module);
    QCOMPARE(copy.key(), module.key());
    QCOMPARE(copy.name(), module.name());
    QCOMPARE(copy.version(), module.version());
    QCOMPARE(copy.targets(), module.targets());
}

void DistroModuleTest::inheritedDistroStruct() {
    DistroModule module("pkg");

    // DistroModule inherits the default output dirs from DistroStruct.
    QVERIFY(!module.isEmpty());
    QVERIFY(!module.getBinOutDir().isEmpty());
    QVERIFY(!module.getLibOutDir().isEmpty());

    module.setBinOutDir("custom-bin");
    module.setLibOutDir("custom-lib");
    QCOMPARE(module.getBinOutDir(), QString("/custom-bin/"));
    QCOMPARE(module.getLibOutDir(), QString("/custom-lib/"));
}
// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(DistroModuleTest)
