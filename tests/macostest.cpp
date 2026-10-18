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
 *  from the given architecture slices. By default the fat header and the
 *  fat_arch entries are big endian (FAT_MAGIC), the layout that lipo writes.
 *  With @a cigam the header is written in the FAT_CIGAM form (little
 *  endian), which is also a valid on-disk representation.
 */
std::vector<unsigned char> makeFatMachO(const QList<QByteArray> &slices,
                                        bool cigam = false) {
    std::vector<unsigned char> f;

    auto put32 = [&](uint32_t x) {
        if (cigam) {
            put32le(f, x);
        } else {
            put32be(f, x);
        }
    };

    // The magic value is the same for both byte orders, only the byte order
    // of the storage differs (put32 handles it): FAT_MAGIC is big endian and
    // FAT_CIGAM is the little endian (byte swapped) form.
    put32(0xCAFEBABEu); // FAT_MAGIC / FAT_CIGAM
    put32(static_cast<uint32_t>(slices.size())); // nfat_arch

    uint32_t offset = 8u + 20u * static_cast<uint32_t>(slices.size());
    for (const auto &slice : slices) {
        put32(7u); // cputype
        put32(3u); // cpusubtype
        put32(offset);
        put32(static_cast<uint32_t>(slice.size()));
        put32(12u); // align
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

void MacOSTest::machOMagicCheck() {
    const auto dir = _tmp.path();

    // All Mach-O magics in both byte orders and the fat magics.
    const auto magicFiles = QList<QPair<QByteArray, bool>>{
        {QByteArray::fromHex("feedface"), true}, // MH_MAGIC LE
        {QByteArray::fromHex("feedfacf"), true}, // MH_MAGIC_64 LE
        {QByteArray::fromHex("cefaedfe"), true}, // MH_MAGIC BE
        {QByteArray::fromHex("cffaedfe"), true}, // MH_MAGIC_64 BE
        {QByteArray::fromHex("cafebabe"), true}, // FAT_MAGIC
        {QByteArray::fromHex("bebafeCA"), true}, // FAT_CIGAM
    };

    int i = 0;
    for (const auto &entry : magicFiles) {
        auto file = dir + "/magic_" + QString::number(i++);
        QVERIFY(writeFile(file, entry.first + QByteArray(8, '\0')));
        QCOMPARE(Extracter::isMachOFile(file), entry.second);
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

    // The dyld reference keeps the Name.framework/Versions/V/Name tail.
    QCOMPARE(DeployCore::macOSFrameworkReference(versionDir + "/QtCore"),
             QStringLiteral("QtCore.framework/Versions/A/QtCore"));
    QCOMPARE(DeployCore::macOSFrameworkReference(dir + "/plain/libfoo.dylib"),
             QStringLiteral("libfoo.dylib"));

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

    // The same fat binary in the FAT_CIGAM form (little endian fields).
    auto fatCigam = makeFatMachO({toBytes(le), toBytes(be)}, true);
    auto fatCigamFile = dir + "/fatCigam.dylib";
    QVERIFY(writeFile(fatCigamFile, toBytes(fatCigam)));
    QCOMPARE(Extracter::machORPaths(fatCigamFile), fatRpaths);
    QVERIFY(Extracter::isMachOFile(fatCigamFile));

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

void MacOSTest::normalizeMacOSRPathCheck() {
    // The trailing slashes are removed, the rest of the path is untouched.
    QCOMPARE(DeployCore::normalizeMacOSRPath(QStringLiteral("/usr/local/lib")),
             QStringLiteral("/usr/local/lib"));
    QCOMPARE(DeployCore::normalizeMacOSRPath(QStringLiteral("/usr/local/lib/")),
             QStringLiteral("/usr/local/lib"));
    QCOMPARE(DeployCore::normalizeMacOSRPath(QStringLiteral("@executable_path/")),
             QStringLiteral("@executable_path"));
    QCOMPARE(DeployCore::normalizeMacOSRPath(QStringLiteral("@loader_path/../bin//")),
             QStringLiteral("@loader_path/../bin"));
    QCOMPARE(DeployCore::normalizeMacOSRPath(QString()), QString());
}

void MacOSTest::planMacOSChangesCheck() {
    const auto dir = _tmp.path();
    const auto libDir = dir + "/lib";
    QVERIFY(QDir().mkpath(libDir));

    // A QtCore.framework binary that links a QtWidgets.framework and a
    // system library.
    const auto idName = "/opt/Qt/6.10.3/macos/lib/QtCore.framework/Versions/A/QtCore";
    const auto qwidgets =
            "/opt/Qt/6.10.3/macos/lib/QtWidgets.framework/Versions/A/QtWidgets";
    auto bytes = makeThinMachODylib(idName, {qwidgets, "/usr/lib/libz.dylib"},
                                    {"/opt/Qt/6.10.3/macos/lib"});
    auto file = libDir + "/QtCore";
    QVERIFY(writeFile(file, toBytes(bytes)));

    // The references map as built by the deployMacOSRPaths: normalized
    // names and file names of the deployed libraries mapped to the
    // @rpath references of the deployed layout.
    QHash<QString, QString> references;
    references.insert(QStringLiteral("QTWIDGETS.FRAMEWORK/VERSIONS/A/QTWIDGETS"),
                      QStringLiteral(
                              "@rpath/QtWidgets.framework/Versions/A/QtWidgets"));
    references.insert(QStringLiteral("QTCORE"),
                      QStringLiteral("@rpath/QtCore.framework/Versions/A/QtCore"));
    references.insert(QStringLiteral("QTCORE2"),
                      QStringLiteral("@rpath/QtCore2.framework/Versions/A/QtCore2"));

    // The libraries are deployed next to the binary, so the rpath is
    // @executable_path.
    auto changes = Extracter::planMacOSChanges(file, references, libDir);
    QCOMPARE(changes.newInstallName,
             QStringLiteral("@rpath/QtCore.framework/Versions/A/QtCore"));
    QCOMPARE(changes.changedLibs.size(), 1);
    QCOMPARE(changes.changedLibs.at(0).first, qwidgets);
    QCOMPARE(changes.changedLibs.at(0).second,
             QStringLiteral("@rpath/QtWidgets.framework/Versions/A/QtWidgets"));
    QCOMPARE(changes.addedRPaths, QStringList{QStringLiteral("@executable_path")});

    // The binary already has the needed rpath, so no rpath is added.
    auto bytes2 = makeThinMachODylib(idName, {qwidgets, "/usr/lib/libz.dylib"},
                                     {"@executable_path"});
    auto file2 = libDir + "/QtCore2";
    QVERIFY(writeFile(file2, toBytes(bytes2)));
    auto changes2 = Extracter::planMacOSChanges(file2, references, libDir);
    QCOMPARE(changes2.newInstallName,
             QStringLiteral("@rpath/QtCore2.framework/Versions/A/QtCore2"));
    QVERIFY(changes2.addedRPaths.isEmpty());

    // The library directory is a sibling of the binary directory, so the
    // rpath has to point there.
    auto changes3 = Extracter::planMacOSChanges(file, references, dir + "/lib2");
    QCOMPARE(changes3.addedRPaths,
             QStringList{QStringLiteral("@executable_path/../lib2")});

    // A non Mach-O file does not produce any change.
    auto elfFile = dir + "/elf";
    QVERIFY(writeFile(elfFile, QByteArray::fromHex("7f454c460000")));
    auto changes4 = Extracter::planMacOSChanges(elfFile, references, libDir);
    QVERIFY(changes4.newInstallName.isEmpty());
    QVERIFY(changes4.changedLibs.isEmpty());
    QVERIFY(changes4.addedRPaths.isEmpty());

    // A missing file does not produce any change.
    auto changes5 = Extracter::planMacOSChanges(dir + "/missing", references, libDir);
    QVERIFY(changes5.newInstallName.isEmpty());
    QVERIFY(changes5.changedLibs.isEmpty());
    QVERIFY(changes5.addedRPaths.isEmpty());
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(MacOSTest)