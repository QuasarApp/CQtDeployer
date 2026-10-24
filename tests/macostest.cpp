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

#include <Distributions/qif.h>
#include <deployconfig.h>
#include <distromodule.h>
#include <filemanager.h>
#include <packagecontrol.h>
#include <params.h>

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

void MacOSTest::dmgQifToolsCheck() {
    // The CQtDeployer.json is the main configuration of the Qt Installer
    // Framework for all platforms (linux, windows and mac, issue #884):
    // it has 2 packages (cqtdeployer and QIF) and after the install all
    // of this will be installed into one target folder, only this config
    // must generate the installer (the qif template + the qifOut name).
    //
    // The CQtDeployerSingle.json is the one package version for DEB and
    // ZIP only (issue #884): it must NOT generate the Qt Installer
    // Framework installer. The "extraData" entry of that config bundles
    // the Qt Installer Framework tools (the QIF folder with binarycreator
    // + installerbase, staged by the copyQIFW CMake target, issue #867)
    // into the same package as the executable - the same rule the flat
    // zip and deb artifacts use (issue #885), so the installed tool
    // finds the binarycreator in the <AppPath>/QIF search path of the
    // QIF distribution when the user creates an installer.
    //
    // The multi-package self-deploy config (CQtDeployer.json) keeps the
    // QIF tools in the separate "QIF" package (the Qt Installer
    // Framework component of the installer) and does not enable the dmg
    // option on its own (issue #885).
#ifdef CQT_DEPLOYER_SELF_DEPLOY_SINGLE_CONFIG
    // The flat single config: the QIF tools bundle (the extraData entry),
    // the deb + zip rules. The dmg option and the qifOut entry must not
    // be part of the config - the flat single package is the one package
    // version for DEB and ZIP only, it must not generate the Qt Installer
    // Framework installer (issue #884).
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
             "folder (./QIFW/QIF) into the flat package, the deb and zip "
             "artifacts would lack binarycreator and installerbase "
             "(issue #885)");

    // The flat single package is the one package version for DEB and ZIP
    // only (issue #884): the deb + zip options are enabled...
    QVERIFY2(config.value("deb").toBool(),
             "The single self-deploy config does not enable the deb option "
             "(the one package version for DEB and ZIP, issue #884)");
    QVERIFY2(config.value("zip").toBool(),
             "The single self-deploy config does not enable the zip option "
             "(the one package version for DEB and ZIP, issue #884)");

    // ...and the Qt Installer Framework installer must not be generated
    // from the flat config: the dmg option and the qifOut entry are not
    // part of the config (issue #884). The installer is generated only
    // from the multi-package config (the CQtDeployer.json).
    QVERIFY2(!config.contains("dmg"),
             "The single self-deploy config must not contain the dmg "
             "option, the flat single package is the DEB and ZIP version "
             "only (issue #884)");
    QVERIFY2(!config.contains("qifOut"),
             "The single self-deploy config must not contain the qifOut "
             "entry, the Qt Installer Framework installer is generated "
             "only from the multi-package config (issue #884)");
#else
    qInfo() << "The single self-deploy config check is skipped, the "
               "CQT_DEPLOYER_SELF_DEPLOY_SINGLE_CONFIG definition is not set";
#endif

#ifdef CQT_DEPLOYER_SELF_DEPLOY_CONFIG
    // The multi-package config: the main configuration of the Qt
    // Installer Framework for all platforms (linux, windows and mac,
    // issue #884): it has 2 packages (cqtdeployer and QIF) and after
    // the install all of this will be installed into one target folder,
    // only this config generates the installer (the qif template + the
    // qifOut name). The dmg option must not be enabled - the installer
    // is generated by the qif option (issue #884, issue #885).
    QFile multiConfigFile(CQT_DEPLOYER_SELF_DEPLOY_CONFIG);
    QVERIFY2(multiConfigFile.open(QIODevice::ReadOnly),
             "The self-deploy config of the build is not found");
    const auto multiConfig =
            QJsonDocument::fromJson(multiConfigFile.readAll()).object();
    multiConfigFile.close();
    QVERIFY2(!multiConfig.isEmpty(),
             "The self-deploy config of the build is not a valid JSON object");
    QVERIFY2(!multiConfig.value("dmg").toBool(),
             "The multi-package self-deploy config must not enable the dmg "
             "option, the Qt Installer Framework installer is generated by "
             "the qif option (issue #884)");

    // The qif template + the qifOut name of the main QtIFW config for
    // all platforms (issue #884).
    const auto qifTemplate = multiConfig.value("qif").toString();
    QVERIFY2(!qifTemplate.isEmpty() && qifTemplate != "false",
             "The multi-package self-deploy config has no Qt Installer "
             "Framework template (the qif entry, the main QtIFW config "
             "for all platforms, issue #884)");
    QVERIFY2(!multiConfig.value("qifOut").toString().isEmpty(),
             "The multi-package self-deploy config has no Qt Installer "
             "Framework output name (the qifOut entry, issue #884)");

    // The cqt wrapper script is bundled into the CQtDeployer.1_6 package
    // (the extraData entry), the installscript.js of the template copies
    // it to the cqt name and links it into the private bin folder of the
    // macOS hosts (issue #884).
    bool hasCqtWrapper = false;
    const auto multiExtraData = multiConfig.value("extraData");
    if (multiExtraData.isArray()) {
        for (const auto &entry: multiExtraData.toArray()) {
            const auto pair = entry.toArray();
            if (pair.size() >= 2 &&
                pair.at(0).toString() == QStringLiteral("CQtDeployer.1_6") &&
                pair.at(1).toString() == QStringLiteral("./QIFWTemplate/res/cqt.sh")) {
                hasCqtWrapper = true;
            }
        }
    }
    QVERIFY2(hasCqtWrapper,
             "The multi-package self-deploy config does not bundle the cqt "
             "wrapper script (./QIFWTemplate/res/cqt.sh) into the "
             "CQtDeployer.1_6 package, the cqt console command of the "
             "macOS hosts is not created (issue #884)");
#else
    qInfo() << "The multi self-deploy config check is skipped, the "
               "CQT_DEPLOYER_SELF_DEPLOY_CONFIG definition is not set";
#endif

    // The package layout the QIF distribution (the dmg option is an
    // alias for the qtifw option, issue #884) stages in the target
    // directory for the flat self-deploy package: the executable and
    // the QIF tools folder (the extraData entry) in the same package
    // (issue #885).
    const auto dir = _tmp.path();
    const auto srcDir = dir + "/src";
    const auto targetDir = dir + "/dist";

    const auto packageDir = srcDir + "/CQtDeployer";
    QDir().mkpath(packageDir + "/QIF");
    QVERIFY(writeFile(packageDir + "/CQtDeployer", QByteArray("dummy")));
    QVERIFY(writeFile(packageDir + "/QIF/binarycreator", QByteArray("dummy")));
    QVERIFY(writeFile(packageDir + "/QIF/installerbase", QByteArray("dummy")));

    DistroModule module("CQtDeployer");
    module.setName("CQtDeployer");
    module.addTarget("CQtDeployer");

    DeployConfig cfg;
    cfg.setTargetDir(targetDir);
    cfg.setDefaultPackage("CQtDeployer");
    cfg.packagesEdit().insert("CQtDeployer", module);

    const auto previousConfig = DeployCore::_config;
    DeployCore::_config = &cfg;

    // The dmg alias: the name of the Qt Installer Framework installer
    // is taken from the qifOut value of the deployment parameters
    // (the same way the qtifw option names it, issue #884).
    QuasarAppUtils::Params::setEnable("dmg", true);
    QuasarAppUtils::Params::setArg("qifOut", "CQtDeployer.dmg");

    FileManager fm;
    QIF qif(&fm);
    TestPackageControl pkg(srcDir, {"CQtDeployer"});

    QVERIFY2(qif.deployTemplate(pkg),
             "The QIF distribution failed to stage the flat self-deploy package");

    // The QIF tools are staged next to the executable in the data of
    // the package, so the QIF distribution finds them in the
    // <AppPath>/QIF search path of the toolKitEnv method when the
    // installed tool creates an installer (issue #885).
    const auto data = targetDir + "/defaultQIFWTemplate/packages/CQtDeployer/data";
    QVERIFY2(QFileInfo(data + "/CQtDeployer").isFile(),
             qPrintable("The package executable is not found: " +
                         data + "/CQtDeployer"));
    QVERIFY2(QFileInfo(data + "/QIF/binarycreator").isFile(),
             qPrintable("binarycreator is not staged into the package data "
                         "(issue #885): " + data + "/QIF/binarycreator"));
    QVERIFY2(QFileInfo(data + "/QIF/installerbase").isFile(),
             qPrintable("installerbase is not staged into the package data "
                         "(issue #885): " + data + "/QIF/installerbase"));

    // The dmg alias names the installer by the qifOut value (on the
    // macOS hosts the binarycreator appends the .app suffix to the
    // installer name, issue #738).
    const auto outFiles = qif.outPutFiles();
    QCOMPARE(outFiles.size(), 1);
#ifdef Q_OS_MACOS
    QVERIFY(outFiles.contains(targetDir + "/CQtDeployer.dmg.app"));
#else
    QVERIFY(outFiles.contains(targetDir + "/CQtDeployer.dmg"));
#endif

    // The removeTemplate step cleans the whole template directory.
    QVERIFY(qif.removeTemplate());
    QVERIFY(!QDir(targetDir + "/defaultQIFWTemplate").exists());

    QuasarAppUtils::Params::setEnable("dmg", false);
    QuasarAppUtils::Params::setArg("qifOut", QString());
    DeployCore::_config = previousConfig;
}

void MacOSTest::macOsInstallScriptCheck() {
    // The installer template of the CQtDeployer self-deploy (the
    // QIFWTemplate folder of the repository) creates the cqt +
    // cqtdeployer symlinks of the private bin folder of the macOS hosts
    // so the commands are accessible from the macOS console (issue #884).
    // The install target dir keeps the default of the installer
    // template (the review of the PR #894).
#ifdef CQT_DEPLOYER_QIFW_TEMPLATE_INSTALLSCRIPT
    QFile installScriptFile(CQT_DEPLOYER_QIFW_TEMPLATE_INSTALLSCRIPT);
    QVERIFY2(installScriptFile.open(QIODevice::ReadOnly),
             "The installscript.js of the QIFW template is not found");
    const auto installScript = QString::fromUtf8(installScriptFile.readAll());
    installScriptFile.close();
    QVERIFY2(!installScript.isEmpty(),
             "The installscript.js of the QIFW template is empty");

    // The macOS branch of the install script (the osx kernel type).
    QVERIFY2(installScript.contains(QStringLiteral("systemInfo.kernelType === \"osx\"")),
             "The installscript.js has no macOS (osx) branch, the cqt and "
             "cqtdeployer console commands of the macOS hosts are not "
             "created (issue #884)");

    // The cqtdeployer symlink of the private bin folder (the Linux
    // pattern, the ~/.profile PATH).
    QVERIFY2(installScript.contains(QStringLiteral("homeDir + \"/.local/bin/cqtdeployer\"")),
             "The cqtdeployer console symlink of the private bin folder is "
             "missing in the installscript.js");

    // The cqt console command of the macOS hosts: the cqt wrapper script
    // (staged into the package by the extraData entry of the self-deploy
    // config) is copied to the cqt name and linked into the private bin
    // folder (issue #884).
    QVERIFY2(installScript.contains(QStringLiteral("cqt.sh")),
             "The cqt wrapper script (cqt.sh) is not installed by the "
             "installscript.js, the cqt console command of the macOS hosts "
             "is missing (issue #884)");
    QVERIFY2(installScript.contains(QStringLiteral("homeDir + \"/.local/bin/cqt\"")),
             "The cqt console symlink of the private bin folder is missing "
             "in the installscript.js (issue #884)");
#else
    qInfo() << "The installscript.js check is skipped, the "
               "CQT_DEPLOYER_QIFW_TEMPLATE_INSTALLSCRIPT definition is not set";
#endif
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:
// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(MacOSTest)