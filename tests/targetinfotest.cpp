//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "targetinfotest.h"
#include "targetinfo.h"

#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>

namespace {

/**
 * @brief minimalMachO This function build a minimal valid Mach-O 64-bit
 *  (little endian) dylib with one LC_ID_DYLIB load command. The layout
 *  is valid enough for the LIEF parser of the DeployCore, so the file
 *  is detected as a Mach-O binary (issue #166).
 */
QByteArray minimalMachO() {
    QByteArray d;
    auto u32 = [&d](quint32 v) {
        d.append(static_cast<char>(v & 0xFFu));
        d.append(static_cast<char>((v >> 8) & 0xFFu));
        d.append(static_cast<char>((v >> 16) & 0xFFu));
        d.append(static_cast<char>((v >> 24) & 0xFFu));
    };

    const QByteArray name = "/opt/Qt/6.10.3/macos/lib/QtCore";
    u32(0xFEEDFACFu); // MH_MAGIC_64
    u32(7u);          // cputype (x86_64)
    u32(3u);          // cpusubtype
    u32(2u);          // filetype (MH_DYLIB)
    u32(1u);          // ncmds
    u32(20u + static_cast<quint32>(name.size()) + 1u); // sizeofcmds
    u32(0u);          // flags
    u32(0u);          // reserved
    u32(0x0000000Du); // LC_ID_DYLIB
    u32(20u + static_cast<quint32>(name.size()) + 1u); // cmdsize
    u32(20u);         // name offset
    u32(0x00010000u); // current version
    u32(0x00010000u); // compatible version
    d.append(name);
    d.append('\0');
    return d;
}

} // namespace

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

    // A suffix-less Mach-O binary gets the macOS run script, which points
    // the dynamic loader to the deployed libraries with the DYLD_*
    // variables, so the binary is not rewritten (issue #166). A
    // suffix-less non Mach-O binary (for example a Linux executable)
    // keeps the linux script.
    QTemporaryFile machoFile;
    QVERIFY(machoFile.open());
    QVERIFY(machoFile.write(minimalMachO()) > 0);
    machoFile.close();

    target.setName(machoFile.fileName());
    QCOMPARE(target.getRunScript(),
             QString(":/Scripts/ScriptsTemplates/macos.sh"));

    QTemporaryFile linuxFile;
    QVERIFY(linuxFile.open());
    QVERIFY(linuxFile.write(QByteArray::fromHex("7f454c460000")) > 0);
    linuxFile.close();

    target.setName(linuxFile.fileName());
    QCOMPARE(target.getRunScript(),
             QString(":/Scripts/ScriptsTemplates/linux.sh"));

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
