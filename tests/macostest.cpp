//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "macostest.h"
#include "deploycore.h"
#include "extracter.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <vector>

namespace {

void put32le(std::vector<unsigned char> &v, uint32_t x) {
    for (int i = 0; i < 4; ++i) {
        v.push_back(static_cast<unsigned char>((x >> (8 * i)) & 0xFFu));
    }
}

void put32be(std::vector<unsigned char> &v, uint32_t x) {
    for (int i = 3; i >= 0; --i) {
        v.push_back(static_cast<unsigned char>((x >> (8 * i)) & 0xFFu));
    }
}

QByteArray toBytes(const std::vector<unsigned char> &v) {
    return QByteArray(reinterpret_cast<const char *>(v.data()),
                      static_cast<int>(v.size()));
}

bool writeFile(const QString &path, const QByteArray &data) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    return file.write(data) == data.size();
}
/**
 * @brief makeThinMachODylib This function build a minimal Mach-O 64-bit
 *  dylib with one LC_ID_DYLIB install name, the LC_LOAD_DYLIB entries of
 *  @a libs and the LC_RPATH entries of @a rpaths. The layout is valid
 *  enough for the LIEF parser and for the machORPaths reader.
 */
std::vector<unsigned char> makeThinMachODylib(
        const QByteArray &idName,
        const QList<QByteArray> &libs,
        const QList<QByteArray> &rpaths,
        bool bigEndian = false) {
    std::vector<unsigned char> f;

    auto put32 = [&](uint32_t x) {
        if (bigEndian) {
            put32be(f, x);
        } else {
            put32le(f, x);
        }
    };

    auto putStr = [&](const QByteArray &s) {
        for (const auto c : s) {
            f.push_back(static_cast<unsigned char>(c));
        }
        f.push_back(0);
    };

    // The magic value is the same for both byte orders, only the byte
    // order of the storage differs (put32 handles it).
    put32(0xFEEDFACFu); // MH_MAGIC_64
    put32(7u); // cputype (x86_64)
    put32(3u); // cpusubtype
    put32(2u); // filetype (MH_DYLIB)
    put32(1u + static_cast<uint32_t>(libs.size())
          + static_cast<uint32_t>(rpaths.size())); // ncmds
    const size_t sizeofcmdsOff = f.size();
    put32(0u); // sizeofcmds (patched below)
    put32(0u); // flags
    put32(0u); // reserved
    uint32_t sizeofcmds = 0;

    auto addDylib = [&](uint32_t cmd, const QByteArray &name) {
        put32(cmd);
        // Layout of the dylib_command: cmd, cmdsize, name offset and the
        // version fields. The name string follows the five header fields.
        const uint32_t cmdSize = 20u + static_cast<uint32_t>(name.size()) + 1u;
        put32(cmdSize);
        put32(20u); // name offset
        put32(0x00010000u); // current version
        put32(0x00010000u); // compatible version
        putStr(name);
        sizeofcmds += cmdSize;
    };

    auto addRpath = [&](const QByteArray &path) {
        put32(0x00001C01u); // LC_RPATH
        const uint32_t cmdSize = 8u + static_cast<uint32_t>(path.size()) + 1u;
        put32(cmdSize);
        putStr(path);
        sizeofcmds += cmdSize;
    };

    addDylib(0x0000000Du, idName); // LC_ID_DYLIB
    for (const auto &lib : libs) {
        addDylib(0x0000000Cu, lib); // LC_LOAD_DYLIB
    }
    for (const auto &rpath : rpaths) {
        addRpath(rpath);
    }

    for (int i = 0; i < 4; ++i) {
        f[sizeofcmdsOff + i] = static_cast<unsigned char>(
                    (sizeofcmds >> (8 * (bigEndian ? 3 - i : i))) & 0xFFu);
    }

    return f;
}

/**
 * @brief makeFatMachO This function build a fat (universal) Mach-O binary
 *  from the given architecture slices. The fat header and the fat_arch
 *  entries are always big endian (FAT_MAGIC), the layout that lipo writes.
 *  The little endian FAT_CIGAM form is not supported, because the LIEF
 *  submodule only parses the canonical big endian fat layout.
 */
std::vector<unsigned char> makeFatMachO(const QList<QByteArray> &slices) {
    std::vector<unsigned char> f;

    put32be(f, 0xCAFEBABEu); // FAT_MAGIC
    put32be(f, static_cast<uint32_t>(slices.size())); // nfat_arch

    uint32_t offset = 8u + 20u * static_cast<uint32_t>(slices.size());
    for (const auto &slice : slices) {
        put32be(f, 7u); // cputype
        put32be(f, 3u); // cpusubtype
        put32be(f, offset);
        put32be(f, static_cast<uint32_t>(slice.size()));
        put32be(f, 12u); // align
        offset += static_cast<uint32_t>(slice.size());
    }

    for (const auto &slice : slices) {
        for (const auto c : slice) {
            f.push_back(static_cast<unsigned char>(c));
        }
    }

    return f;
}



} // namespace

void MacOSTest::initTestCase() {
    QVERIFY(_tmp.isValid());
}

void MacOSTest::cleanupTestCase() {
}

void MacOSTest::machODetectionCheck() {
    const auto dir = _tmp.path();

    // Valid Mach-O binaries in the layouts the LIEF parser accepts:
    // thin 64-bit in both byte orders and a fat binary with two slices.
    auto leFile = dir + "/valid_le";
    QVERIFY(writeFile(leFile, toBytes(makeThinMachODylib(
                "@rpath/libLe.dylib", {}, {"/opt/le/lib"}))));
    QVERIFY(Extracter::isMachOFile(leFile));

    auto beFile = dir + "/valid_be";
    QVERIFY(writeFile(beFile, toBytes(makeThinMachODylib(
                "@rpath/libBe.dylib", {}, {"/opt/be/lib"}, true))));
    QVERIFY(Extracter::isMachOFile(beFile));

    auto fatFile = dir + "/valid_fat";
    QVERIFY(writeFile(fatFile, toBytes(makeFatMachO({
                toBytes(makeThinMachODylib(
                        "@rpath/libLe.dylib", {}, {"/opt/le/lib"})),
                toBytes(makeThinMachODylib(
                        "@rpath/libBe.dylib", {}, {"/opt/be/lib"}, true))}))));
    QVERIFY(Extracter::isMachOFile(fatFile));

    // The detection is a full file parsing on the LIEF backend, so a file
    // that only contains a Mach-O magic (without a valid header) is not a
    // Mach-O binary anymore (issue #166).
    const auto magicFiles = QList<QByteArray>{
        QByteArray::fromHex("feedface"), // MH_MAGIC LE
        QByteArray::fromHex("feedfacf"), // MH_MAGIC_64 LE
        QByteArray::fromHex("cefaedfe"), // MH_MAGIC BE
        QByteArray::fromHex("cffaedfe"), // MH_MAGIC_64 BE
        QByteArray::fromHex("cafebabe"), // FAT_MAGIC
        QByteArray::fromHex("bebafeCA"), // FAT_CIGAM
    };

    int i = 0;
    for (const auto &magic : magicFiles) {
        auto file = dir + "/magic_only_" + QString::number(i++);
        QVERIFY(writeFile(file, magic + QByteArray(8, '\0')));
        QVERIFY(!Extracter::isMachOFile(file));
    }

    // Not a Mach-O file.
    auto elf = dir + "/elf";
    QVERIFY(writeFile(elf, QByteArray::fromHex("7f454c460000")));
    QVERIFY(!Extracter::isMachOFile(elf));

    // Too short / empty / missing files.
    auto shortFile = dir + "/short";
    QVERIFY(writeFile(shortFile, QByteArray("fee")));
    QVERIFY(!Extracter::isMachOFile(shortFile));

    auto emptyFile = dir + "/empty";
    QVERIFY(writeFile(emptyFile, QByteArray()));
    QVERIFY(!Extracter::isMachOFile(emptyFile));

    QVERIFY(!Extracter::isMachOFile(dir + "/missing"));
}

void MacOSTest::frameworkHelpers() {
    const auto dir = _tmp.path();
    auto framework = dir + "/QtCore.framework";
    auto versionDir = framework + "/Versions/A";

    QVERIFY(QDir().mkpath(versionDir));
    QFile bin(versionDir + "/QtCore");
    QVERIFY(bin.open(QIODevice::WriteOnly));
    bin.write("fake-mach-o-content");
    bin.close();

    // Versions/Current -> A and the top level binary symlink. Note that
    // QProcess::execute returns the exit code, so success is 0.
    QCOMPARE(QProcess::execute("ln",
                               {"-sfn", "A", framework + "/Versions/Current"}),
             0);
    QCOMPARE(QProcess::execute("ln",
                               {"-sfn", "Versions/Current/QtCore",
                                framework + "/QtCore"}),
             0);

    // The framework root is found for the binary and for the bundle.
    QCOMPARE(DeployCore::macOSFrameworkRoot(versionDir + "/QtCore"), framework);
    QCOMPARE(DeployCore::macOSFrameworkRoot(framework), framework);
    QVERIFY(DeployCore::macOSFrameworkRoot(dir + "/plain/libfoo.dylib").isEmpty());

    // The real binary of the bundle is found in the real version directory
    // (not through the Current symlink), because the dyld install names of
    // the Qt frameworks are based on that path.
    QCOMPARE(DeployCore::macOSFrameworkBinary(framework),
             framework + "/Versions/A/QtCore");
    QVERIFY(DeployCore::macOSFrameworkBinary(dir + "/plain").isEmpty());

    // Only suffix-less files inside a framework count as framework
    // binaries.
    QVERIFY(DeployCore::isMacOSFrameworkBinary(QFileInfo(versionDir + "/QtCore")));
    QVERIFY(DeployCore::isMacOSFrameworkBinary(QFileInfo(framework + "/QtCore")));
    QVERIFY(!DeployCore::isMacOSFrameworkBinary(
                QFileInfo(versionDir + "/QtCore.dylib")));
    QVERIFY(!DeployCore::isMacOSFrameworkBinary(QFileInfo(dir + "/QtCore")));

    // The install name normalization strips the dyld prefixes and keeps
    // the framework tail.
    QCOMPARE(DeployCore::normalizeMacOSLibName(
                 QStringLiteral("@rpath/libQt6Core.6.dylib")),
             QStringLiteral("libQt6Core.6.dylib"));
    QCOMPARE(DeployCore::normalizeMacOSLibName(
                 QStringLiteral(
                         "@loader_path/../QtWidgets.framework/Versions/A/QtWidgets")),
             QStringLiteral("QtWidgets.framework/Versions/A/QtWidgets"));
    QCOMPARE(DeployCore::normalizeMacOSLibName(
                 QStringLiteral(
                         "/opt/homebrew/lib/QtCore.framework/Versions/A/QtCore")),
             QStringLiteral("QtCore.framework/Versions/A/QtCore"));
    QCOMPARE(DeployCore::normalizeMacOSLibName(
                 QStringLiteral("@executable_path/libz.1.dylib")),
             QStringLiteral("libz.1.dylib"));
    QCOMPARE(DeployCore::normalizeMacOSLibName(
                 QStringLiteral("QtCore.framework/Versions/A/QtCore")),
             QStringLiteral("QtCore.framework/Versions/A/QtCore"));
}

void MacOSTest::machORPathsCheck() {
    const auto dir = _tmp.path();

    // Thin little endian dylib with two rpaths.
    auto le = makeThinMachODylib(
                "/opt/Qt/6.10.3/macos/lib/QtCore.framework/Versions/A/QtCore",
                {"/opt/Qt/6.10.3/macos/lib/QtWidgets.framework/Versions/A/QtWidgets"},
                {"/opt/Qt/6.10.3/macos/lib", "@executable_path"});
    auto leFile = dir + "/le.dylib";
    QVERIFY(writeFile(leFile, toBytes(le)));
    QCOMPARE(Extracter::machORPaths(leFile),
             (QStringList{QStringLiteral("/opt/Qt/6.10.3/macos/lib"),
                          QStringLiteral("@executable_path")}));
    QVERIFY(Extracter::isMachOFile(leFile));

    // Thin big endian dylib.
    auto be = makeThinMachODylib("/opt/Qt/6.10.3/macos/lib/QtCore",
                                 {}, {"/usr/lib"}, true);
    auto beFile = dir + "/be.dylib";
    QVERIFY(writeFile(beFile, toBytes(be)));
    QCOMPARE(Extracter::machORPaths(beFile), QStringList{QStringLiteral("/usr/lib")});
    QVERIFY(Extracter::isMachOFile(beFile));

    // Fat binary with one little endian and one big endian slice. The
    // rpaths of both slices are expected. The fat header uses the canonical
    // FAT_MAGIC layout (big endian fields).
    auto fat = makeFatMachO({toBytes(le), toBytes(be)});
    auto fatFile = dir + "/fat.dylib";
    QVERIFY(writeFile(fatFile, toBytes(fat)));
    auto fatRpaths = Extracter::machORPaths(fatFile);
    QVERIFY(fatRpaths.contains(QStringLiteral("/opt/Qt/6.10.3/macos/lib")));
    QVERIFY(fatRpaths.contains(QStringLiteral("@executable_path")));
    QVERIFY(fatRpaths.contains(QStringLiteral("/usr/lib")));
    QCOMPARE(fatRpaths.size(), 3);
    QVERIFY(Extracter::isMachOFile(fatFile));

    // No rpaths at all.
    auto noRpath = makeThinMachODylib("/opt/Qt/6.10.3/macos/lib/QtCore",
                                      {"/usr/lib/libz.dylib"}, {});
    auto noRpathFile = dir + "/norpath.dylib";
    QVERIFY(writeFile(noRpathFile, toBytes(noRpath)));
    QVERIFY(Extracter::machORPaths(noRpathFile).isEmpty());

    // Not a Mach-O file.
    auto elfFile = dir + "/elf";
    QVERIFY(writeFile(elfFile, QByteArray::fromHex("7f454c460000")));
    QVERIFY(Extracter::machORPaths(elfFile).isEmpty());

    auto emptyFile = dir + "/empty";
    QVERIFY(writeFile(emptyFile, QByteArray()));
    QVERIFY(Extracter::machORPaths(emptyFile).isEmpty());

    QVERIFY(Extracter::machORPaths(dir + "/missing").isEmpty());
}


// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(MacOSTest)