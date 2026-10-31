//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//


#include "dmgtest.h"

#include <deploycore.h>

#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QTextStream>

#include <Distributions/dmg.h>
#include <deployconfig.h>
#include <distromodule.h>
#include <filemanager.h>
#include <packagecontrol.h>

namespace {

/**
 * @brief findDeployerExe This function returns the path to the
 *  cqtdeployer executable of the current build tree. The build tree
 *  executable is set by the tests CMakeLists.txt (the
 *  DMG_TEST_CQT_DEPLOYER_EXE compile definition), so the tests cover
 *  the code under review and not the last stable release installed by
 *  the CI action. The cqtdeployer of the system PATH is used as a
 *  fallback.
 */
QString findDeployerExe() {
#ifdef DMG_TEST_CQT_DEPLOYER_EXE
    const QString buildTreeExe = QString::fromLatin1(DMG_TEST_CQT_DEPLOYER_EXE);
    if (!buildTreeExe.isEmpty() && QFileInfo::exists(buildTreeExe)) {
        return buildTreeExe;
    }
#endif
    const QStringList names = {
        QStringLiteral("cqtdeployer"),
        QStringLiteral("CQtDeployer")
    };
    for (const auto &name: names) {
        const QString found = QStandardPaths::findExecutable(name);
        if (!found.isEmpty()) {
            return found;
        }
    }

    return QString();
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

bool writeFile(const QString &path, const QByteArray &data) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    return file.write(data) == data.size();
}

} // namespace

void DmgTest::initTestCase() {
    _deployerExe = findDeployerExe();
}

void DmgTest::cleanupTestCase() {
}

QString DmgTest::distDirListing() const {
    const QDir dist(_tmp.path() + "/dist");
    if (!dist.exists()) {
        return QStringLiteral("<missing>");
    }
    QStringList list;
    QDirIterator it(dist.absolutePath(),
                    QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    while (it.hasNext()) {
        list << it.next();
    }
    return list.join(QStringLiteral(", "));
}

bool DmgTest::dmgIsUdif(const QFileInfo &dmg) {
    // The DMG files of the hdiutil are the UDIF (the Universal Disk
    // Image Format) images: the file starts and ends with the UDIF
    // trailer - the 512 bytes block with the "koly" magic (issue #899).
    QFile file(dmg.filePath());
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    const QByteArray data = file.readAll();
    file.close();
    if (data.size() < 512) {
        return false;
    }
    return data.right(512).startsWith(QByteArray("koly"));
}
void DmgTest::optionTableCheck() {
    // The dmg + the dmgOut options are registered in the options table
    // of the DeployCore (the same check pattern as the AppImage and the
    // Qifw tests, issue #899).
    const auto options = DeployCore::avilableOptions();

    const QuasarAppUtils::OptionData *dmgOption = nullptr;
    const QuasarAppUtils::OptionData *dmgOutOption = nullptr;
    for (const auto &option: options) {
        if (option.names().contains("dmg")) {
            dmgOption = &option;
        }
        if (option.names().contains("-dmgOut")) {
            dmgOutOption = &option;
        }
    }

    // The dmg option is registered (the DMG backend of issue #899).
    QVERIFY2(dmgOption != nullptr,
             "The dmg option is not registered in the options table");
    if (dmgOption != nullptr) {
        QCOMPARE(dmgOption->names(), (QStringList() << "dmg"));
        QVERIFY(dmgOption->description().contains("hdiutil"));
        QVERIFY(dmgOption->description().contains("macOS"));
    }

    // The dmgOut option is registered (the name of the output file,
    // the same rule as the debOut/zipOut/appimageOut options).
    QVERIFY2(dmgOutOption != nullptr,
             "The dmgOut option is not registered in the options table");
    if (dmgOutOption != nullptr) {
        QCOMPARE(dmgOutOption->names(), (QStringList() << "-dmgOut"));
        QVERIFY(dmgOutOption->description().contains("DMG"));
    }
}

void DmgTest::dmgDistributionStagingCheck() {
    // The Dmg distribution of the Deploy library (issue #899): the
    // staging of the DMG volume (the deployed data of the package, the
    // icon, the README file with the package information and the
    // Applications symbolic link), the hdiutil command of the runCmd
    // method, the outPutFiles and the removeTemplate method. The test
    // runs on every platform: the staging logic is platform independent
    // and the runCmd method only builds the hdiutil command line (the
    // real hdiutil run is covered by the end-to-end test of the macOS
    // hosts).
    const auto dir = _tmp.path();
    const auto srcDir = dir + "/srcStaging";
    const auto targetDir = dir + "/distStaging";

    // The deployed data of the package: the executable + the run script
    // of the ready-to-use application.
    const auto packageDir = srcDir + "/TestApp";
    QDir().mkpath(packageDir + "/bin");
    QVERIFY(writeFile(packageDir + "/bin/TestApp",
                      QByteArray("the test executable")));
    QVERIFY(writeFile(packageDir + "/bin/TestApp.sh",
                      QByteArray("#!/bin/sh\nexec ./TestApp\n")));

    // The default name of the output file: the <name>.dmg default of
    // the collectInfo method (issue #899).
    {
        DistroModule module("TestApp");
        module.setName("TestApp");
        module.setDescription("The test description of the TestApp package.");
        module.setVersion("1.2.3");
        module.setPublisher("The Test Publisher");
        module.setHomePage("https://quasarapp.github.io/CQtDeployer/");
        module.addTarget("TestApp");

        DeployConfig cfg;
        cfg.setTargetDir(targetDir);
        cfg.setDefaultPackage("TestApp");
        cfg.packagesEdit().insert("TestApp", module);

        const auto previousConfig = DeployCore::_config;
        DeployCore::_config = &cfg;

        FileManager fm;
        Dmg dmg(&fm);
        TestPackageControl pkg(srcDir, {"TestApp"});

        QVERIFY2(dmg.deployTemplate(pkg),
                 "The Dmg distribution failed to stage the DMG volume");

        // The root of the DMG volume: the template location of the
        // Dmg distribution + the name of the package (the same pattern
        // as the deb and the appimage distributions).
        const auto root = targetDir + "/defaultDmgTemplate/TestApp";

        // The deployed data of the package is moved to the application
        // folder of the volume (the ready-to-use application).
        QVERIFY2(QFileInfo(root + "/TestApp/bin/TestApp").isFile(),
                 qPrintable("The deployed data is not found in the "
                            "application folder of the DMG volume: " +
                            root));
        QVERIFY2(QFileInfo(root + "/TestApp/bin/TestApp.sh").isFile(),
                 qPrintable("The run script of the package is not found "
                            "in the DMG volume: " + root));

        // The icon of the application: the targets of the test
        // configuration are not registered in the targets table, so the
        // default icon of the CQtDeployer (the png icon of the Deploy
        // resources) is the fallback (the same rule as the AppImage
        // distribution, issue #898).
        const QFileInfo iconInfo(root + "/TestApp.png");
        QVERIFY2(iconInfo.isFile(),
                 qPrintable("The icon of the application is not found in "
                            "the DMG volume: " + root));
        QVERIFY2(iconInfo.size() > 0,
                 qPrintable("The icon of the application is empty: " +
                            iconInfo.filePath()));

        // The README file of the volume: the name, the description, the
        // version, the publisher and the homepage of the package (the
        // placeholders of the template are replaced, the same mechanism
        // as the deb control template).
        const QFileInfo readmeInfo(root + "/README");
        QVERIFY2(readmeInfo.isFile(),
                 qPrintable("The README file is not found in the DMG "
                            "volume: " + root));
        {
            QFile readme(readmeInfo.filePath());
            QVERIFY2(readme.open(QIODevice::ReadOnly),
                     "Failed to open the README file of the DMG volume");
            const auto content = QString::fromUtf8(readme.readAll());
            readme.close();
            QVERIFY2(content.contains("TestApp"),
                     "The README file has no name of the package");
            QVERIFY2(content.contains("The test description of the TestApp package."),
                     "The README file has no description of the package");
            QVERIFY2(content.contains("1.2.3"),
                     "The README file has no version of the package");
            QVERIFY2(content.contains("The Test Publisher"),
                     "The README file has no publisher of the package");
            QVERIFY2(content.contains("https://quasarapp.github.io/CQtDeployer/"),
                     "The README file has no homepage of the package");
            QVERIFY2(!content.contains("$NAME") &&
                         !content.contains("$DESCRIPTION"),
                     "The placeholders of the README template are not "
                     "replaced");
        }

        // The Applications symbolic link of the standard drag-to-install
        // UX of the macOS DMG files (issue #899). The QFile::link
        // creates the .lnk shortcuts on the Windows hosts (the .lnk
        // suffix is required), so the symbolic link check is skipped
        // there.
#ifndef Q_OS_WIN32
        QVERIFY2(QFileInfo(root + "/Applications").isSymLink(),
                 qPrintable("The Applications symbolic link is not found in "
                            "the DMG volume: " + root));
#else
        qInfo() << "The Applications symbolic link check is skipped on the "
                   "Windows hosts (the QFile::link creates the .lnk shortcuts there)";
#endif

        // The hdiutil create command of the DMG (issue #899): the
        // -volname value is the name of the package, the -srcfolder
        // value is the root of the volume, the -ov flag overwrites the
        // existing image and the UDBZ format is the compressed UDIF of
        // the standard macOS DMG files.
        const auto commands = dmg.runCmd();
        QCOMPARE(commands.size(), 1);
        QCOMPARE(commands.first().arguments.size(), 10);
        QCOMPARE(commands.first().arguments.at(0),
                 QStringLiteral("create"));
        QCOMPARE(commands.first().arguments.at(1),
                 QStringLiteral("-volname"));
        QCOMPARE(commands.first().arguments.at(2),
                 QStringLiteral("TestApp"));
        QCOMPARE(commands.first().arguments.at(3),
                 QStringLiteral("-srcfolder"));
        QCOMPARE(commands.first().arguments.at(4), root);
        QCOMPARE(commands.first().arguments.at(5),
                 QStringLiteral("-ov"));
        QCOMPARE(commands.first().arguments.at(6),
                 QStringLiteral("-format"));
        QCOMPARE(commands.first().arguments.at(7),
                 QStringLiteral("UDBZ"));
        QCOMPARE(commands.first().arguments.at(8),
                 QStringLiteral("-o"));
        QCOMPARE(commands.first().arguments.at(9),
                 targetDir + "/TestApp.dmg");

        // The name of the output file (the <name>.dmg default).
        const auto outFiles = dmg.outPutFiles();
        QCOMPARE(outFiles.size(), 1);
        QCOMPARE(outFiles.first(), targetDir + "/TestApp.dmg");

        // The removeTemplate step cleans the whole template directory.
        QVERIFY(dmg.removeTemplate());
        QVERIFY(!QDir(targetDir + "/defaultDmgTemplate").exists());

        DeployCore::_config = previousConfig;
    }

    // The dmgOut option of the package: the name of the output file
    // without the .dmg suffix (the suffix is appended by the Dmg
    // distribution, the same way the hdiutil appends it) and with the
    // .dmg suffix (the name is kept, issue #899).
    {
        DistroModule module("TestApp");
        module.setName("TestApp");
        module.setDmgOut("CustomDmgName");
        module.addTarget("TestApp");

        DeployConfig cfg;
        cfg.setTargetDir(targetDir);
        cfg.setDefaultPackage("TestApp");
        cfg.packagesEdit().insert("TestApp", module);

        const auto previousConfig = DeployCore::_config;
        DeployCore::_config = &cfg;

        FileManager fm;
        Dmg dmg(&fm);
        TestPackageControl pkg(srcDir, {"TestApp"});

        QVERIFY2(dmg.deployTemplate(pkg),
                 "The Dmg distribution failed to stage the DMG volume");
        QCOMPARE(dmg.outPutFiles().first(),
                 targetDir + "/CustomDmgName.dmg");
        QVERIFY(dmg.removeTemplate());

        DeployCore::_config = previousConfig;
    }
    {
        DistroModule module("TestApp");
        module.setName("TestApp");
        module.setDmgOut("CustomDmg.dmg");
        module.addTarget("TestApp");

        DeployConfig cfg;
        cfg.setTargetDir(targetDir);
        cfg.setDefaultPackage("TestApp");
        cfg.packagesEdit().insert("TestApp", module);

        const auto previousConfig = DeployCore::_config;
        DeployCore::_config = &cfg;

        FileManager fm;
        Dmg dmg(&fm);
        TestPackageControl pkg(srcDir, {"TestApp"});

        QVERIFY2(dmg.deployTemplate(pkg),
                 "The Dmg distribution failed to stage the DMG volume");
        QCOMPARE(dmg.outPutFiles().first(),
                 targetDir + "/CustomDmg.dmg");
        QVERIFY(dmg.removeTemplate());

        DeployCore::_config = previousConfig;
    }
}

void DmgTest::dmgOptionEndToEndCheck() {
    // The dmg option of the cqtdeployer of the current build tree
    // (issue #899): on the macOS hosts the option creates the real
    // DMG package with the hdiutil tool of the system, on the other
    // host platforms the option is skipped with a warning (the same
    // rule as the "deb" and the "appimage" options) and no DMG output
    // is created. The dmg option is not an alias for the qtifw option
    // anymore (issue #884), so no Qt Installer Framework installer is
    // created for it.
    if (_deployerExe.isEmpty()) {
        QSKIP("The cqtdeployer executable of the build tree is not found, the dmg test cannot run");
    }

    // The dmg and the noQt options are flags without the "-" prefix:
    // every option with the "-" prefix consumes the next token as its
    // value (the QuasarApp Params parser).
    const QString textFileName = QStringLiteral("clinerules");
    const QDir srcDir(_tmp.path() + "/src");
    QVERIFY2(srcDir.mkpath(QStringLiteral(".")), "Failed to create the source dir");

    const QString textFilePath = srcDir.absolutePath() + "/" + textFileName;
    {
        QFile f(textFilePath);
        QVERIFY2(f.open(QIODevice::WriteOnly | QIODevice::Text),
                 "Failed to create the general text file");
        f.write("A general text file used by the issue #899 regression test.\n");
    }

    int exitCode = -1;
    QString output;
    runDeployerProcess({
        "clear",
        "-bin", textFilePath,
        "noQt",
        "dmg",
        "-targetDir", _tmp.path() + "/dist",
        "-dmgOut", "DmgOptionTest"
    }, QStringLiteral("dmgOption"), &exitCode, &output);
    QCOMPARE(exitCode, 0);

    const QFileInfo dmg(_tmp.path() + "/dist/DmgOptionTest.dmg");
#ifdef Q_OS_MACOS
    // The hdiutil appends the .dmg suffix to the -dmgOut value (the
    // same way the Dmg distribution normalizes the name, issue #899).
    QVERIFY2(dmg.isFile(),
             qPrintable("The DMG package of the dmg option is not created. "
                        "The dist dir content: [" + distDirListing() +
                        "]. The cqtdeployer output (the full output is "
                        "stored in test_report.txt.dmgOption):\n" + output));
    QVERIFY2(dmg.size() > 0,
             qPrintable("The DMG package of the dmg option is empty: " +
                        dmg.filePath()));
    QVERIFY2(dmgIsUdif(dmg),
             qPrintable("The DMG package of the dmg option is not a valid "
                        "UDIF image (the hdiutil format of the macOS DMG "
                        "files): " + dmg.filePath()));
#else
    QVERIFY2(output.contains("The dmg option is available only on the macOS host platform"),
             qPrintable("The warning of the dmg option (the macOS host "
                        "platforms only, issue #899) is not printed. The "
                        "cqtdeployer output (the full output is stored "
                        "in test_report.txt.dmgOption):\n" + output));
    QVERIFY2(!dmg.exists(),
             qPrintable("The DMG package is created on the non-macOS host "
                        "platform, the dmg option is not skipped: " +
                        dmg.filePath()));
    // The dmg option is not an alias for the qtifw option anymore
    // (issue #884), so no Qt Installer Framework installer is created
    // for it (issue #899).
    QVERIFY2(!output.contains("binarycreator") &&
                 !output.contains("The qtifw package"),
             qPrintable("The dmg option triggered the Qt Installer "
                        "Framework (the old alias of issue #884), the "
                        "dmg option must create the DMG packages only "
                        "(issue #899). The cqtdeployer output:\n" + output));
#endif
}

void DmgTest::runDeployerProcess(const QStringList &args, const QString &runName,
                                 int *exitCode, QString *output) {
    if (exitCode != nullptr) {
        *exitCode = -1;
    }
    if (output != nullptr) {
        *output = QString();
    }

    if (_deployerExe.isEmpty()) {
        if (output != nullptr) {
            *output = QStringLiteral("The cqtdeployer executable is not found");
        }
        return;
    }

    QProcess proc;
    proc.setProcessChannelMode(QProcess::MergedChannels);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("QT_LOGGING_TO_CONSOLE", "1");
    proc.setProcessEnvironment(env);

    proc.start(_deployerExe, args);
    QVERIFY2(proc.waitForStarted(30000), "Failed to start the cqtdeployer");
    QVERIFY2(proc.waitForFinished(5 * 60 * 1000),
             "The cqtdeployer did not finish in time");
    if (exitCode != nullptr) {
        *exitCode = proc.exitCode();
    }
    if (output != nullptr) {
        *output = QString::fromLocal8Bit(proc.readAll());
    }

    // The QtTest report truncates the long failure messages, so the full
    // cqtdeployer output of every run is stored in a separate file next
    // to the test report (the same pattern as the AppImage and the Qifw
    // tests, issue #898).
    QFile report(QCoreApplication::applicationDirPath() + "/test_report.txt." + runName);
    if (report.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&report);
        stream << *output;
    }
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:

// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(DmgTest)

