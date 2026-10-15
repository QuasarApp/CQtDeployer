//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "targetinfotest.h"
#include "targetinfo.h"

void TargetInfoTest::initTestCase() {
}

void TargetInfoTest::cleanupTestCase() {
}

void TargetInfoTest::package() {
    TargetInfo target;
    QVERIFY(target.getPackage().isEmpty());

    target.setPackage("mypkg");
    QCOMPARE(target.getPackage(), QString("mypkg"));
}

void TargetInfoTest::icon() {
    TargetInfo target;

    // If the icon is not set, a default icon from the resources
    // is returned.
    target.setName("app");
    QCOMPARE(target.getIcon(),
             QString(":/shared/Distributions/Templates/Icon.png"));

    target.setName("app.exe");
    QCOMPARE(target.getIcon(),
             QString(":/shared/Distributions/Templates/Icon.ico"));

    target.setIcon("/opt/icons/custom.png");
    QCOMPARE(target.getIcon(), QString("/opt/icons/custom.png"));
}

void TargetInfoTest::shortCut() {
    TargetInfo target;

    // By default the shortcut is enabled for files without a suffix
    // or with a .exe suffix.
    QVERIFY(target.getShortCut());

    target.setName("app");
    QVERIFY(target.getShortCut());

    target.setName("app.so");
    QVERIFY(!target.getShortCut());

    target.setName("app");
    target.setShortCut(false);
    QVERIFY(!target.getShortCut());

    target.setShortCut(true);
    QVERIFY(target.getShortCut());

    target.disableShortCut();
    QVERIFY(!target.getShortCut());
}

void TargetInfoTest::runScript() {
    TargetInfo target;

    // By default the run script is enabled for files without a suffix
    // or with a .exe suffix.
    QVERIFY(target.fEnableRunScript());

    target.setName("app");
    QVERIFY(target.fEnableRunScript());

    target.setName("app.so");
    QVERIFY(!target.fEnableRunScript());

    // If the run script is not set, a default script is returned.
    target.setName("app");
    QCOMPARE(target.getRunScript(),
             QString(":/Scripts/ScriptsTemplates/linux.sh"));

    target.setName("app.exe");
    QCOMPARE(target.getRunScript(),
             QString(":/Scripts/ScriptsTemplates/windows.bat"));

    target.setName("app");
    target.setRunScript("/opt/scripts/custom.sh");
    QCOMPARE(target.getRunScript(), QString("/opt/scripts/custom.sh"));

    target.setFEnableRunScript(false);
    QVERIFY(!target.fEnableRunScript());

    target.setFEnableRunScript(true);
    QVERIFY(target.fEnableRunScript());

    target.disableRunScript();
    QVERIFY(!target.fEnableRunScript());
}

void TargetInfoTest::runScriptFile() {
    TargetInfo target;

    target.setName("app");
    QCOMPARE(target.getRunScriptFile(), QString("app.sh"));

    target.setName("app.exe");
    QCOMPARE(target.getRunScriptFile(), QString("app.bat"));

    target.setName("app");
    target.setRunScript("custom-script.sh");
    QCOMPARE(target.getRunScriptFile(), QString("app.sh"));

    target.disableRunScript();
    QCOMPARE(target.getRunScriptFile(), QString("app"));
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(TargetInfoTest)
