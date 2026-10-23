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
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QtEndian>
#include <QtGlobal>
#include <vector>

#include <Distributions/dmg.h>
#include <deployconfig.h>
#include <distromodule.h>
#include <filemanager.h>
#include <packagecontrol.h>

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
 * @brief TestPackageControl This is a PackageControl implementation for
 *  the unit tests: it copies the package folders of the @a sourceDir
 *  directory (one subfolder per package key) to the new location, the
 *  same way the package data is staged by the real Packing class
 *  (issue #166).
 */
class TestPackageControl: public PackageControl {
public:
    TestPackageControl(const QString &sourceDir,
                       const QStringList &packages) :
        _sourceDir(sourceDir),
        _packages(packages)
    {
    }

    bool movePackage(const QString &package,
                     const QString &newLocation) override {
        return copyPackage(package, newLocation);
    }

    bool copyPackage(const QString &package,
                     const QString &newLocation) override {
        const auto source = _sourceDir + "/" + package;
        QDir targetDir(newLocation);
        if (!targetDir.exists() && !targetDir.mkpath(".")) {
            return false;
        }

        QDirIterator it(source, QDir::Files | QDir::Dirs | QDir::Hidden,
                        QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const auto file = QFileInfo(it.next());
            const auto rel = file.absoluteFilePath().mid(source.size() + 1);
            const auto dest = newLocation + "/" + rel;
            if (file.isDir()) {
                if (!QDir().mkpath(dest)) {
                    return false;
                }
            } else if (!QFile::copy(file.absoluteFilePath(), dest)) {
                return false;
            }
        }

        return true;
    }

    QStringList availablePackages() const override {
        return _packages;
    }

private:
    QString _sourceDir;
    QStringList _packages;
};
/**
 * @brief udzoKolyHeaderOffset returns the offset of the 512-byte "koly"
 *  header of the compressed (UDZO) disk image @a image. The hdiutil
 *  create tool writes the zlib compressed partition data first and the
 *  header last (at the file size minus the header size offset), the old
 *  images can have the header at the beginning (issue #166). The header
 *  must carry the "koly" magic, the version 4.0, the zlib compression
 *  method (2, the "ZO" of the UDZO format) and the no-encryption (0)
 *  fields, the same signature the 7-Zip DmgHandler checks.
 *  Returns -1, when the image does not contain a valid UDZO koly header.
 */
qint64 udzoKolyHeaderOffset(const QByteArray &image) {
    static const auto kSignature =
            QByteArray::fromHex("6b6f6c790000000400000200");
    static const auto kHeaderSize = 512;
    if (image.size() < kHeaderSize) {
        return -1;
    }
    if (image.startsWith(kSignature)) {
        return 0;
    }
    const auto tail = image.size() - kHeaderSize;
    return image.mid(tail, kSignature.size()) == kSignature ? tail : -1;
}
/**
 * @brief makeThinMachODylib This function build a minimal Mach-O 64-bit
 *  dylib with one LC_ID_DYLIB install name, the LC_LOAD_DYLIB entries of
 *  @a libs and the LC_RPATH entries of @a rpaths. The layout is valid
 *  enough for the LIEF parser and for the machORPaths reader. The
 *  @a fileType is the Mach-O file type (2 = MH_DYLIB, 1 = MH_EXECUTE).
 */
std::vector<unsigned char> makeThinMachODylib(
        const QByteArray &idName,
        const QList<QByteArray> &libs,
        const QList<QByteArray> &rpaths,
        bool bigEndian = false,
        uint32_t fileType = 2u) {
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
    put32(fileType); // filetype (MH_DYLIB/MH_EXECUTE)
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
#if defined(Q_OS_WIN)
    // The cross deployment for the macOS targets is supported from the
    // Linux hosts only (on the macOS hosts the deployment is native), a
    // Windows host can not deploy for the macOS targets, so the tests
    // of this class are not useful there (issue #166).
    QSKIP("The macOS deployment tests are not supported on the Windows hosts");
#endif
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

void MacOSTest::dmgDistributionCheck() {
    FileManager fm;
    Dmg dmg(&fm);

    // The Info.plist template of the application bundle is embedded in
    // the resources of the Deploy library (issue #166).
    QFile plist(":/Templates/DMG/Distributions/Templates/dmg/Contents/Info.plist");
    QVERIFY(plist.exists());

    // The DMG distribution is only available on the macOS hosts with
    // the hdiutil tool (issue #166).
#ifdef Q_OS_MACOS
    QVERIFY(dmg.isAvailable());
#else
    QVERIFY(!dmg.isAvailable());
#endif
}

void MacOSTest::dmgOutConfigCheck() {
    DistroModule module("test");

    // The DMG output name is empty by default, the Dmg distribution
    // derives the default "<name>.dmg" file name in this case.
    QVERIFY(module.dmgOut().isEmpty());

    module.setDmgOut("custom.dmg");
    QCOMPARE(module.dmgOut(), QStringLiteral("custom.dmg"));
}

void MacOSTest::dmgMultiPackageCheck() {
    // The Dmg distribution creates a separate DMG archive for every
    // available package, the same way the deb distribution creates a
    // separate .deb file for every available package (issue #166).
    const auto dir = _tmp.path();
    const auto srcDir = dir + "/src";
    const auto targetDir = dir + "/dist";

    auto makePackage = [&](const char *key,
                           const char *executable,
                           const char *name) {
        QDir().mkpath(srcDir + "/" + key);
        writeFile(srcDir + "/" + key + "/" + executable,
                  QByteArray("dummy"));

        DistroModule module(key);
        // The DMG distribution is created on the macOS hosts, where the
        // default deploy layout keeps the files in the package root
        // (the same layout the macOS DistroStruct defaults define).
        module.setBinOutDir("/");
        module.setLibOutDir("/");
        module.setName(name);
        module.addTarget(executable);

        return module;
    };

    auto app1 = makePackage("app1", "App1", "First App");
    auto app2 = makePackage("app2", "App2", "Second App");
    app2.setDmgOut("custom2.dmg");

    // The extra-data package (docs) has no executable files, but it is
    // packed as a separate DMG archive too (the same way the extra-data
    // packages get their own .deb files in the deb distribution, issue
    // #166).
    QDir().mkpath(srcDir + "/docs");
    QVERIFY(writeFile(srcDir + "/docs/readme.txt", QByteArray("docs")));
    DistroModule docs("docs");

    DeployConfig cfg;
    cfg.setTargetDir(targetDir);
    cfg.packagesEdit().insert("app1", app1);
    cfg.packagesEdit().insert("app2", app2);
    cfg.packagesEdit().insert("docs", docs);

    const auto previousConfig = DeployCore::_config;
    DeployCore::_config = &cfg;

    FileManager fm;
    Dmg dmg(&fm);
    TestPackageControl pkg(srcDir, {"app1", "app2", "docs"});

    QVERIFY2(dmg.deployTemplate(pkg),
             "The Dmg distribution failed to stage the multi-package deployment");

    // A separate DMG archive for every available package: the default
    // "<name>.dmg" file names and the per-package "dmgOut" value (the
    // same way as the .deb output names of the deb distribution, issue
    // #166).
    const auto outFiles = dmg.outPutFiles();
    QCOMPARE(outFiles.size(), 3);
    QVERIFY(outFiles.contains(targetDir + "/First App.dmg"));
    QVERIFY(outFiles.contains(targetDir + "/custom2.dmg"));
    QVERIFY(outFiles.contains(targetDir + "/docs.dmg"));

    // Every bundle is staged in its own folder of the DMG template
    // directory: the executable in the MacOS folder of the bundle and
    // the substituted Info.plist of the package (issue #166).
    const QList<QStringList> bundles = {
        {"app1", "App1", "First App"},
        {"app2", "App2", "Second App"}
    };
    for (const auto &bundle: bundles) {
        const auto key = bundle.value(0);
        const auto exe = bundle.value(1);
        const auto name = bundle.value(2);

        const auto bundleDir =
                targetDir + "/defaultDMGTemplate/" + key + "/" + exe + ".app";
        QVERIFY2(QFileInfo(bundleDir + "/Contents/MacOS/" + exe).isFile(),
                 qPrintable("The bundle executable is not found: " +
                            bundleDir + "/Contents/MacOS/" + exe));

        QFile plistFile(bundleDir + "/Contents/Info.plist");
        QVERIFY2(plistFile.open(QIODevice::ReadOnly),
                 qPrintable("The bundle Info.plist is not found: " +
                            bundleDir + "/Contents/Info.plist"));
        const auto content = QString::fromUtf8(plistFile.readAll());
        // The file must be closed before the removeTemplate step,
        // Windows does not allow to delete a file with the open handle
        // (issue #166).
        plistFile.close();
        QVERIFY2(content.contains(name),
                 qPrintable("The bundle Info.plist has no package name: " +
                            bundleDir + "/Contents/Info.plist"));
        QVERIFY(!content.contains("$CQT_BUNDLE_ID"));
        QVERIFY(!content.contains("$CQT_BUNDLE_EXECUTABLE"));
        QVERIFY(content.contains("org.quasarapp." + exe.toLower()));
        QVERIFY(content.contains(exe));
    }

    // The extra-data package is staged as a data only bundle: the data
    // in the MacOS folder, the substituted Info.plist without the
    // executable name (issue #166).
    const auto docsBundle = targetDir + "/defaultDMGTemplate/docs/docs.app";
    QVERIFY2(QFileInfo(docsBundle + "/Contents/MacOS/readme.txt").isFile(),
             "The docs package data is not staged in the bundle");
    QFile docsPlist(docsBundle + "/Contents/Info.plist");
    QVERIFY2(docsPlist.open(QIODevice::ReadOnly),
             "The docs bundle Info.plist is not found");
    const auto docsContent = QString::fromUtf8(docsPlist.readAll());
    // The file must be closed before the removeTemplate step,
    // Windows does not allow to delete a file with the open handle
    // (issue #166).
    docsPlist.close();
    QVERIFY(!docsContent.contains("$CQT_BUNDLE_ID"));
    QVERIFY(!docsContent.contains("$CQT_BUNDLE_EXECUTABLE"));
    QVERIFY(docsContent.contains("docs"));

    // The "Applications" symbolic link of every DMG stage allows to
    // drag the bundle to the Applications folder on the mounted image
    // (the macOS hosts only, issue #166).
#ifdef Q_OS_UNIX
    const QStringList stages = {"app1", "app2", "docs"};
    for (const auto &stage: stages) {
        const auto link =
                targetDir + "/defaultDMGTemplate/" + stage + "/Applications";
        QVERIFY(QFileInfo(link).isSymLink());
    }
#endif

    // The runCmd step runs the "hdiutil create" command for every
    // prepared stage (one DMG archive per package, issue #166), the
    // install_name_tool step is run only for the bundles with the
    // executable files.
    int hdiutilCount = 0;
    int rpathCount = 0;
    for (const auto &cmd: dmg.runCmd()) {
        if (cmd.command == "hdiutil" && cmd.arguments.value(0) == "create") {
            ++hdiutilCount;
        }
        if (cmd.command == "install_name_tool") {
            ++rpathCount;
        }
    }
    QCOMPARE(hdiutilCount, 3);
    QCOMPARE(rpathCount, 2);

    // The removeTemplate step cleans the whole template directory
    // (issue #166).
    QVERIFY(dmg.removeTemplate());
    QVERIFY(!QDir(targetDir + "/defaultDMGTemplate").exists());

    DeployCore::_config = previousConfig;
}

void MacOSTest::dmgQifToolsCheck() {
    // The DMG archive of the CQtDeployer self-deploy is created from the
    // flat single package of the CQtDeployerSingle.json config: the
    // "extraData" entry of that config bundles the Qt Installer
    // Framework tools (the QIF folder with binarycreator + installerbase,
    // staged by the copyQIFW CMake target, issue #867) into the same
    // package as the executable - the same rule the flat zip and deb
    // artifacts use (issue #885). The Dmg distribution copies the
    // package content into the MacOS folder of the bundle, so the DMG
    // archive contains the QIF folder with the tools and the executable
    // of the bundle finds them in the <AppPath>/QIF search path of the
    // QIF distribution when the user creates an installer on the macOS
    // hosts.
    //
    // The multi-package self-deploy config (CQtDeployer.json) keeps the
    // QIF tools in the separate "QIF" package (the Qt Installer
    // Framework component of the installer) and does not create the DMG
    // archive on its own: the DMG archive created from that config would
    // lack the QIF tools (the reported bug, issue #885).
#ifdef CQT_DEPLOYER_SELF_DEPLOY_SINGLE_CONFIG
    // The flat single config: the QIF tools bundle (the extraData
    // entry) and the DMG rules (the dmg flag + the dmgOut name).
    QFile configFile(CQT_DEPLOYER_SELF_DEPLOY_SINGLE_CONFIG);
    QVERIFY2(configFile.open(QIODevice::ReadOnly),
             "The single self-deploy config of the build is not found");
    const auto config = QJsonDocument::fromJson(configFile.readAll()).object();
    configFile.close();
    QVERIFY2(!config.isEmpty(),
             "The single self-deploy config of the build is not a valid JSON object");

    // The QIF tools folder is bundled into the package of the flat
    // config (the extraData entry, the same rule the flat zip and deb
    // artifacts use, issue #885).
    bool hasQifTools = false;
    const auto extraData = config.value("extraData");
    if (extraData.isString()) {
        hasQifTools = extraData.toString() == QStringLiteral("./QIFW/QIF");
    } else if (extraData.isArray()) {
        for (const auto &entry: extraData.toArray()) {
            const auto pair = entry.toArray();
            for (int i = 1; i < pair.size(); ++i) {
                if (pair.at(i).toString() == QStringLiteral("./QIFW/QIF")) {
                    hasQifTools = true;
                }
            }
        }
    }
    QVERIFY2(hasQifTools,
             "The single self-deploy config does not bundle the QIF tools "
             "folder (./QIFW/QIF) into the flat package, the DMG archive "
             "would lack binarycreator and installerbase (issue #885)");

    // The DMG rules are part of the config (the CMake DMG_ENABLED
    // placeholder is resolved per platform: enabled on the macOS hosts,
    // disabled on all other platforms where the hdiutil tool does not
    // exist, issue #166).
    QVERIFY2(config.contains("dmg"),
             "The single self-deploy config has no DMG rule (issue #885)");
#ifdef Q_OS_MACOS
    QVERIFY2(config.value("dmg").toBool(),
             "The single self-deploy config does not enable the DMG "
             "distribution on the macOS hosts (issue #885)");
#endif
    QVERIFY2(!config.value("dmgOut").toString().isEmpty(),
             "The single self-deploy config has no DMG output name (issue #885)");
#else
    qInfo() << "The single self-deploy config check is skipped, the "
               "CQT_DEPLOYER_SELF_DEPLOY_SINGLE_CONFIG definition is not set";
#endif

#ifdef CQT_DEPLOYER_SELF_DEPLOY_CONFIG
    // The multi-package config: the DMG distribution must not be
    // enabled - the DMG archive is created from the flat single package
    // that bundles the QIF tools (issue #885).
    QFile multiConfigFile(CQT_DEPLOYER_SELF_DEPLOY_CONFIG);
    QVERIFY2(multiConfigFile.open(QIODevice::ReadOnly),
             "The self-deploy config of the build is not found");
    const auto multiConfig =
            QJsonDocument::fromJson(multiConfigFile.readAll()).object();
    multiConfigFile.close();
    QVERIFY2(!multiConfig.isEmpty(),
             "The self-deploy config of the build is not a valid JSON object");
    QVERIFY2(!multiConfig.value("dmg").toBool(),
             "The multi-package self-deploy config must not create the DMG "
             "archive, the DMG archive is created from the flat single "
             "package that bundles the QIF tools (issue #885)");
#else
    qInfo() << "The multi self-deploy config check is skipped, the "
               "CQT_DEPLOYER_SELF_DEPLOY_CONFIG definition is not set";
#endif

    // The package layout the flat self-deploy creates in the target
    // directory (the macOS deploy layout keeps the executable and the
    // libraries in the package root, the same layout the macOS
    // DistroStruct defaults define): the executable and the QIF tools
    // folder (the extraData entry) in the same package (issue #885).
    const auto dir = _tmp.path();
    const auto srcDir = dir + "/src";
    const auto targetDir = dir + "/dist";

    const auto packageDir = srcDir + "/CQtDeployer";
    QDir().mkpath(packageDir + "/QIF");
    QVERIFY(writeFile(packageDir + "/CQtDeployer", QByteArray("dummy")));
    QVERIFY(writeFile(packageDir + "/QIF/binarycreator", QByteArray("dummy")));
    QVERIFY(writeFile(packageDir + "/QIF/installerbase", QByteArray("dummy")));

    DistroModule module("CQtDeployer");
    module.setBinOutDir("/");
    module.setLibOutDir("/");
    module.setName("CQtDeployer");
    module.setDmgOut("CQtDeployer.dmg");
    module.addTarget("CQtDeployer");

    DeployConfig cfg;
    cfg.setTargetDir(targetDir);
    cfg.packagesEdit().insert("CQtDeployer", module);

    const auto previousConfig = DeployCore::_config;
    DeployCore::_config = &cfg;

    FileManager fm;
    Dmg dmg(&fm);
    TestPackageControl pkg(srcDir, {"CQtDeployer"});

    QVERIFY2(dmg.deployTemplate(pkg),
             "The Dmg distribution failed to stage the flat self-deploy package");

    // The QIF tools are staged into the MacOS folder of the bundle (the
    // QIF folder of the DMG archive), so the QIF distribution finds them
    // in the <AppPath>/QIF search path of the toolKitEnv method (issue
    // #885).
    const auto bundleMacOS =
            targetDir + "/defaultDMGTemplate/CQtDeployer/CQtDeployer.app/Contents/MacOS";
    QVERIFY2(QFileInfo(bundleMacOS + "/CQtDeployer").isFile(),
             qPrintable("The bundle executable is not found: " +
                         bundleMacOS + "/CQtDeployer"));
    QVERIFY2(QFileInfo(bundleMacOS + "/QIF/binarycreator").isFile(),
             qPrintable("binarycreator is not staged into the DMG bundle "
                         "(issue #885): " +
                         bundleMacOS + "/QIF/binarycreator"));
    QVERIFY2(QFileInfo(bundleMacOS + "/QIF/installerbase").isFile(),
             qPrintable("installerbase is not staged into the DMG bundle "
                         "(issue #885): " +
                         bundleMacOS + "/QIF/installerbase"));

    // The DMG output of the staged package (issue #885).
    const auto outFiles = dmg.outPutFiles();
    QCOMPARE(outFiles.size(), 1);
    QVERIFY(outFiles.contains(targetDir + "/CQtDeployer.dmg"));

    // The removeTemplate step cleans the whole template directory.
    QVERIFY(dmg.removeTemplate());
    QVERIFY(!QDir(targetDir + "/defaultDMGTemplate").exists());

    DeployCore::_config = previousConfig;
}

void MacOSTest::dmgCreationCheck() {
#ifndef Q_OS_MACOS
    QSKIP("The DMG distribution is created only on the macOS hosts with the hdiutil tool");
#endif
    const auto dir = _tmp.path();
    const auto stageDir = dir + "/stage";
    const auto bundleDir = stageDir + "/MyApp.app";
    const auto macosDir = bundleDir + "/Contents/MacOS";
    const auto exe = macosDir + "/MyApp";
    QDir().mkpath(macosDir);

    // A real Mach-O executable (MH_EXECUTE) without rpath entries, the
    // same kind of binary the Dmg distribution stages in the bundle.
    // The install_name_tool step below rewrites the file, so it needs
    // a fully valid binary (the segment commands and the free header
    // space to insert the new LC_RPATH load command) that a minimal
    // hand built header does not provide. A copy of the system "true"
    // executable is a real MH_EXECUTE of the host architecture without
    // rpath entries (the runCmd step of the Dmg distribution, issue
    // #166).
    const auto systemExe = QStringLiteral("/usr/bin/true");
    QVERIFY2(QFileInfo(systemExe).isReadable(),
             "The DMG test requires the /usr/bin/true executable of the host");
    QVERIFY(QFile::copy(systemExe, exe));

    // The same commands the Dmg distribution runs (runCmd): the
    // @executable_path rpath of the bundle executable and the
    // compressed (UDZO) DMG archive (issue #166).
    QProcess rpathProc;
    rpathProc.start("install_name_tool", {"-add_rpath", "@executable_path", exe});
    rpathProc.waitForFinished(-1);
    QCOMPARE(rpathProc.exitCode(), 0);

    const auto dmgFile = dir + "/MyApp.dmg";
    QProcess hdi;
    hdi.start("hdiutil", {"create",
                          "-volname", "MyApp",
                          "-srcfolder", stageDir,
                          "-ov",
                          "-format", "UDZO",
                          dmgFile});
    hdi.waitForFinished(-1);
    const auto hdiLog = QString::fromUtf8(hdi.readAllStandardOutput())
                        + QString::fromUtf8(hdi.readAllStandardError());
    QCOMPARE(hdi.exitCode(), 0);

    QFileInfo dmgInfo(dmgFile);
    QVERIFY(dmgInfo.exists());
    QVERIFY(dmgInfo.size() > 0);

    // The created image must be a compressed (UDZO) disk image: the
    // hdiutil create tool writes the zlib compressed partition data
    // first and the 512-byte "koly" header with the compression method
    // field (2 = zlib, the "ZO" of the UDZO format) to the end of the
    // file (the old images can have the header at the beginning,
    // issue #166).
    QFile img(dmgFile);
    QVERIFY2(img.open(QIODevice::ReadOnly), "Failed to open the created DMG archive");
    const auto image = img.readAll();
    img.close();

    const auto headerOffset = udzoKolyHeaderOffset(image);
    QProcess fileCmd;
    fileCmd.start("file", {"-b", dmgFile});
    fileCmd.waitForFinished(-1);
    const auto fileType = QString::fromUtf8(fileCmd.readAllStandardOutput()).trimmed();
    QVERIFY2(headerOffset >= 0,
             qPrintable("The DMG archive is not a compressed (UDZO) koly image. "
                        "size: " + QString::number(image.size())
                        + ", file: " + fileType
                        + ", first 12 bytes: " + QString::fromLatin1(image.left(12).toHex())
                        + ", last 12 bytes: " + QString::fromLatin1(image.right(12).toHex())
                        + ", hdiutil: " + hdiLog));

    // The data fork field of the koly header (the big-endian offset and
    // length at the 0x18 offset of the header) describes the compressed
    // data of the image, the first byte of the fork must be the "78"
    // zlib magic (issue #166).
    const auto *data = reinterpret_cast<const uchar *>(image.constData());
    const auto forkOffset = qFromBigEndian<quint64>(data + headerOffset + 0x18);
    const auto forkLength = qFromBigEndian<quint64>(data + headerOffset + 0x20);
    QVERIFY2(forkLength > 0 && forkOffset + forkLength <= static_cast<quint64>(image.size()),
             qPrintable("The data fork of the DMG koly header is invalid. "
                        "fork offset: " + QString::number(forkOffset)
                        + ", fork length: " + QString::number(forkLength)
                        + ", image size: " + QString::number(image.size())));
    QCOMPARE(static_cast<int>(data[forkOffset]), 0x78); // the zlib magic
}


// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(MacOSTest)