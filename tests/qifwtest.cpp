//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "qifwtest.h"

#include <deploycore.h>

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QTextStream>

namespace {

/**
 * @brief findDeployerExe This function returns the path to the
 *  cqtdeployer executable of the current build tree. The build tree
 *  executable is set by the tests CMakeLists.txt (the
 *  QIFW_TEST_CQT_DEPLOYER_EXE compile definition), so the tests cover the
 *  code under review and not the last stable release installed by the CI
 *  action (issue #738). The cqtdeployer of the system PATH is used as a
 *  fallback.
 */
QString findDeployerExe() {
#ifdef QIFW_TEST_CQT_DEPLOYER_EXE
    const QString buildTreeExe = QString::fromLatin1(QIFW_TEST_CQT_DEPLOYER_EXE);
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
 * @brief installerIsNotEmpty Returns true when the created installer
 *  artifact contains data. The Qt Installer Framework binarycreator
 *  creates a single executable file on the Windows and the Linux hosts,
 *  but on the macOS hosts it creates the installer as an .app bundle - a
 *  directory with the ".app" suffix appended to the qifOut name - so a
 *  non-empty check has to work for files and for directories (issue #738).
 *  For the .app bundle the completeness criterion is the bundle executable
 *  in Contents/MacOS - the standard .app layout the macOS binarycreator
 *  creates (the staged IFW binary contains the "Contents/MacOS" and
 *  "Contents/Info.plist" bundle paths).
 */
bool installerIsNotEmpty(const QFileInfo &installer) {
    if (installer.isDir()) {
        const QDir macosDir(installer.filePath() + "/Contents/MacOS");
        return !macosDir.entryList(QDir::Files | QDir::NoDotAndDotDot).isEmpty();
    }
    return installer.isFile() && installer.size() > 0;
}

} // namespace

void QifwTest::initTestCase() {
    _deployerExe = findDeployerExe();

#ifdef QIFW_TEST_QMAKE
    _qmake = QString::fromLatin1(QIFW_TEST_QMAKE);
#endif

    // The test executable itself is a deployed Qt application (the
    // deployTest target of the QuasarApp CMake CI targets), so it is a
    // valid target of the qtifw option: the installer created for it is a
    // real Qt Installer Framework installer of a Qt application.
    _targetBinary = QCoreApplication::applicationFilePath();
    _targetLibDir = QCoreApplication::applicationDirPath();
}

void QifwTest::cleanupTestCase() {
}

QString QifwTest::installerName(const char *base) const {
    // The QIF::installerFile() method of the Deploy library uses the
    // ".run" suffix on the Linux hosts and the ".exe" suffix on every
    // other platform (including macOS). The qifOut option below overrides
    // the default name, the suffix must match the platform.
#ifdef Q_OS_LINUX
    return QString::fromLatin1(base) + QStringLiteral(".run");
#else
    return QString::fromLatin1(base) + QStringLiteral(".exe");
#endif
}

QFileInfo QifwTest::installerInfo(const char *base) const {
    // On the macOS hosts the Qt Installer Framework binarycreator creates
    // the installer as an .app bundle - a directory with the ".app" suffix
    // appended to the qifOut name (the captured output of the macOS CI
    // hosts shows the "QifwTestInstaller.exe.app" directory next to the
    // deployed "Application" directory, issue #738). On the Windows and
    // the Linux hosts the installer is a single executable file with the
    // qifOut name.
    const QString baseDir = _tmp.path() + "/dist/";
#ifdef Q_OS_MACOS
    return QFileInfo(baseDir + installerName(base) + QStringLiteral(".app"));
#else
    return QFileInfo(baseDir + installerName(base));
#endif
}

void QifwTest::runDeployer(const QStringList &distroOptions, const QString &runName,
                           int *exitCode, QString *output) {
    QStringList args = {
        "clear",
        "-bin", _targetBinary,
        "-targetDir", _tmp.path() + "/dist",
        "-libDir", _targetLibDir,
        "-recursiveDepth", "5",
        "-qifOut", installerName()
    };
    if (!_qmake.isEmpty()) {
        args << "-qmake" << _qmake;
    }
    args << distroOptions;

    runDeployerProcess(args, runName, exitCode, output);
}

void QifwTest::runDeployerProcess(const QStringList &args, const QString &runName,
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

    // The deprecation warning of the qif alias is printed by the cqtdeployer
    // while it parses the command line options - before the QALogger handler
    // of the cqtdeployer main() is installed (issue #875). Until that point
    // the default Qt message handler is active and on the Windows hosts it
    // routes the messages to the debugger instead of the console, so the
    // warning (and the working options table) would be lost in the captured
    // process output. QT_LOGGING_TO_CONSOLE=1 makes the default handler use
    // the console (it is ignored on the non-Windows platforms) (issue #738).
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
    // cqtdeployer output of every run is stored in a separate file next to
    // the test report. The CI uploads all the test_report.txt* files of the
    // test directories, so the full output is available for the failed runs
    // (issue #738).
    QFile dump(QStringLiteral("test_report.txt.") + runName);
    if (dump.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream stream(&dump);
        stream << *output;
    }
}

QString QifwTest::distDirListing() const {
    QDir dir(_tmp.path() + "/dist");
    QStringList lines;
    const auto entries = dir.entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const auto &entry: entries) {
        const QString size = entry.isDir()
            ? QStringLiteral("<dir>")
            : QString::number(entry.size());
        lines << QStringLiteral("%1 (%2)").arg(entry.fileName(), size);
    }
    return lines.join(QStringLiteral(", "));
}

QString QifwTest::artifactListing() const {
    // A listing of the installer artifact itself for the failure messages:
    // the size for the file artifacts of the Windows and the Linux hosts,
    // the top-level entries plus the "Contents" entries for the .app bundle
    // of the macOS hosts. An incomplete bundle (for example an empty
    // ".app" directory) is then visible in the CI log and in the uploaded
    // test reports (issue #738).
    const QFileInfo artifact = installerInfo();
    if (!artifact.exists()) {
        return QStringLiteral("<missing>");
    }
    if (!artifact.isDir()) {
        return QString::number(artifact.size());
    }
    QStringList lines;
    const auto appendLevel = [&lines](const QDir &dir, const QString &prefix) {
        const auto entries = dir.entryInfoList(
            QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        for (const auto &entry: entries) {
            const QString size = entry.isDir()
                ? QStringLiteral("<dir>")
                : QString::number(entry.size());
            lines << QStringLiteral("%1%2 (%3)").arg(prefix, entry.fileName(), size);
        }
    };
    appendLevel(QDir(artifact.filePath()), QString());
    const QDir contentsDir(artifact.filePath() + "/Contents");
    if (contentsDir.exists()) {
        appendLevel(contentsDir, QStringLiteral("Contents/"));
    }
    return lines.join(QStringLiteral(", "));
}

bool QifwTest::waitForInstaller(int timeoutMs, const char *base) {
    // The .app bundle of the macOS hosts is populated by a background step
    // of the binarycreator that outlives the cqtdeployer process (on the
    // macOS CI hosts the "Contents" directory appears in the milliseconds
    // after the cqtdeployer exit, issue #738), so the installer artifact is
    // polled until it is complete instead of being checked once right after
    // the process exit. On the Windows and the Linux hosts the file
    // artifact is complete when the process exits, the first poll returns
    // and the wait is a no-op.
    const qint64 deadline = QDateTime::currentMSecsSinceEpoch() + timeoutMs;
    for (;;) {
        const QFileInfo installer = installerInfo(base);
        if (installer.exists() && installerIsNotEmpty(installer)) {
            return true;
        }
        if (QDateTime::currentMSecsSinceEpoch() >= deadline) {
            return false;
        }
        QTest::qWait(250);
    }
}

void QifwTest::optionTableCheck() {
    const auto options = DeployCore::avilableOptions();

    const QuasarAppUtils::OptionData *qtifwOption = nullptr;
    const QuasarAppUtils::OptionData *qifOption = nullptr;
    for (const auto &option: options) {
        if (option.names().contains("qtifw")) {
            qtifwOption = &option;
        }
        if (option.names().contains("qif")) {
            qifOption = &option;
        }
    }

    // The renamed option is registered with the new name and is not
    // deprecated (issue #738).
    QVERIFY2(qtifwOption != nullptr,
             "The qtifw option is not registered in the options table");
    if (qtifwOption != nullptr) {
        QCOMPARE(qtifwOption->names(),
                 (QStringList() << "qtifw" << "-qtifw"));
        QVERIFY(!qtifwOption->isDepricated());
        QVERIFY(qtifwOption->example().contains("qtifw"));
    }

    // The old name is kept as a deprecated alias: it is still registered
    // and marked as deprecated with the message that points to the new
    // name (issue #738).
    QVERIFY2(qifOption != nullptr,
             "The deprecated qif alias is not registered in the options table");
    if (qifOption != nullptr) {
        QCOMPARE(qifOption->names(), (QStringList() << "qif" << "-qif"));
        QVERIFY(qifOption->isDepricated());
        QVERIFY(qifOption->depricatedMsg().contains("qtifw"));
    }
}

void QifwTest::qtifwInstallerCheck() {
    // The Qt Installer Framework tools are stored in the repository for
    // every supported platform and are staged next to the cqtdeployer of
    // the build tree by the copyQIFW target (issue #867), so the qtifw
    // option is tested on every platform, including the macOS hosts
    // (issue #738).
    if (_deployerExe.isEmpty()) {
        QSKIP("The cqtdeployer executable of the build tree is not found, the qtifw installer test cannot run");
    }

    int exitCode = -1;
    QString output;
    runDeployer({"qtifw"}, QStringLiteral("qtifw"), &exitCode, &output);
    QCOMPARE(exitCode, 0);

    const QFileInfo installer = installerInfo();
    QVERIFY2(waitForInstaller(5 * 60 * 1000),
             qPrintable("The qtifw installer is not created (the wait timed out). "
                        "The dist dir content: [" + distDirListing()
                        + "]. The installer artifact (" + installer.filePath() + ") content: ["
                        + artifactListing()
                        + "]. The cqtdeployer output (the full output is stored in "
                          "test_report.txt.qtifw):\n" + output));
}

void QifwTest::qifDeprecatedAliasCheck() {
    // The legacy "qif" name keeps working on every platform, including
    // the macOS hosts (issue #738), but it prints the deprecation warning
    // that points to the new qtifw option.
    if (_deployerExe.isEmpty()) {
        QSKIP("The cqtdeployer executable of the build tree is not found, the qif alias test cannot run");
    }

    int exitCode = -1;
    QString output;
    runDeployer({"qif"}, QStringLiteral("qif"), &exitCode, &output);
    QCOMPARE(exitCode, 0);

    const QFileInfo installer = installerInfo();
    QVERIFY2(waitForInstaller(5 * 60 * 1000),
             qPrintable("The qif (deprecated alias) installer is not created (the wait "
                        "timed out). The dist dir content: [" + distDirListing()
                        + "]. The installer artifact (" + installer.filePath() + ") content: ["
                        + artifactListing()
                        + "]. The cqtdeployer output (the full output is stored in "
                          "test_report.txt.qif):\n" + output));

    QVERIFY2(output.contains("deprecated"),
             qPrintable("The deprecation warning is not printed. The cqtdeployer output "
                        "(the full output is stored in test_report.txt.qif):\n" + output));
    QVERIFY2(output.contains("renamed to 'qtifw'"),
             qPrintable("The deprecation warning does not point to the qtifw option. "
                        "The cqtdeployer output (the full output is stored in "
                          "test_report.txt.qif):\n" + output));
}

void QifwTest::dmgAliasInstallerCheck() {
    // The dmg option is an alias for the QtIFW installer (issue #884):
    // the same binarycreator installer is created, the output name is
    // taken from the dmgOut value (the extra .run/.exe suffix is not
    // added; the .app suffix of the macOS hosts is appended by the QIF
    // distribution, issue #738).
    if (_deployerExe.isEmpty()) {
        QSKIP("The cqtdeployer executable of the build tree is not found, the dmg alias test cannot run");
    }

    const QString dmgOut = QStringLiteral("QifwDmgAliasInstaller.dmg");

    int exitCode = -1;
    QString output;
    QStringList args = {
        "clear",
        "-bin", _targetBinary,
        "-targetDir", _tmp.path() + "/dist",
        "-libDir", _targetLibDir,
        "-recursiveDepth", "5",
        "dmg",
        "-dmgOut", dmgOut
    };
    if (!_qmake.isEmpty()) {
        args << "-qmake" << _qmake;
    }

    runDeployerProcess(args, QStringLiteral("dmgAlias"), &exitCode, &output);
    QCOMPARE(exitCode, 0);

    // The old Dmg distribution warning must be gone: the dmg option is
    // handled by the QIF distribution (the binarycreator) now, not by
    // the hdiutil tool (issue #884).
    QVERIFY2(!output.contains("hdiutil"),
             qPrintable("The hdiutil warning of the removed Dmg distribution is "
                        "printed, the dmg option is not an alias for the qtifw "
                        "option (issue #884). The cqtdeployer output (the full "
                        "output is stored in test_report.txt.dmgAlias):\n" + output));

    // The dmgOut name of the installer (the .app bundle of the binarycreator
    // on the macOS hosts, issue #738).
    const QString baseDir = _tmp.path() + "/dist/";
    const QFileInfo installer =
#ifdef Q_OS_MACOS
            QFileInfo(baseDir + dmgOut + QStringLiteral(".app"));
#else
            QFileInfo(baseDir + dmgOut);
#endif

    // The .app bundle of the macOS hosts is populated by a background step
    // of the binarycreator that outlives the cqtdeployer process (issue
    // #738), so the installer artifact is polled until it is complete.
    bool ready = false;
    const qint64 deadline = QDateTime::currentMSecsSinceEpoch() + 5 * 60 * 1000;
    while (!ready && QDateTime::currentMSecsSinceEpoch() < deadline) {
        ready = installerIsNotEmpty(QFileInfo(installer.filePath()));
        if (!ready) {
            QTest::qWait(250);
        }
    }

    QVERIFY2(ready,
             qPrintable("The dmg (qtifw alias) installer of the dmgOut name is "
                        "not created (the wait timed out). The dist dir content: ["
                        + distDirListing()
                        + "]. The installer artifact (" + installer.filePath()
                        + "): [" + (installer.exists() ? QString::number(installer.size())
                                                       : QStringLiteral("<missing>"))
                        + "]. The cqtdeployer output (the full output is stored in "
                        "test_report.txt.dmgAlias):\n" + output));
}

void QifwTest::noQtGeneralFileInstallerCheck() {
    // A plain text file passed via the bin option cannot be parsed as a
    // binary of a known format (the ELF/PE/MACHO scanners fail on it).
    // The deployer must register such a target as a general file, copy
    // the file to the target dir and create the installer with it
    // (issue #883).
    if (_deployerExe.isEmpty()) {
        QSKIP("The cqtdeployer executable of the build tree is not found, the qtifw installer test cannot run");
    }

    const QString textFileName = QStringLiteral("clinerules");
    const QDir srcDir(_tmp.path() + "/src");
    QVERIFY2(srcDir.mkpath(QStringLiteral(".")), "Failed to create the source dir");

    const QString textFilePath = srcDir.absolutePath() + "/" + textFileName;
    {
        QFile f(textFilePath);
        QVERIFY2(f.open(QIODevice::WriteOnly | QIODevice::Text),
                 "Failed to create the general text file");
        f.write("A general text file used by the issue #883 regression test.\n");
    }

    int exitCode = -1;
    QString output;
    // The noQt and the qtifw options are flags without the "-" prefix:
    // every option with the "-" prefix consumes the next token as its
    // value (the QuasarApp Params parser), so "-noQt" would swallow the
    // qtifw flag and no installer would be created (issue #883).
    runDeployerProcess({
        "clear",
        "-bin", textFilePath,
        "noQt",
        "qtifw",
        "-targetDir", _tmp.path() + "/dist",
        "-qifOut", installerName("QifwGeneralFileInstaller")
    }, QStringLiteral("noQtGeneralFile"), &exitCode, &output);
    QCOMPARE(exitCode, 0);

    const QFileInfo installer = installerInfo("QifwGeneralFileInstaller");
    QVERIFY2(waitForInstaller(5 * 60 * 1000, "QifwGeneralFileInstaller"),
             qPrintable("The qtifw installer of the general file target is not created "
                        "(the wait timed out). The dist dir content: [" + distDirListing()
                        + "]. The installer artifact (" + installer.filePath()
                        + "): [" + artifactListing()
                        + "]. The cqtdeployer output (the full output is stored in "
                        "test_report.txt.noQtGeneralFile):\n" + output));

    // The smartMoveTargets() method of the ConfigParser copies every target
    // to <targetDir>/<package><binOutDir>/. The package is the "Application"
    // default package and the bin output dir is "/bin" on the Linux hosts
    // and "/" on the other platforms (the DistroStruct defaults), so the
    // general file target has an exact, known location in the target dir.
    // CQtDeployer does not select general files from folders automatically,
    // the test checks the expected path instead of a recursive search
    // (issue #883).
    QString expectedPath = _tmp.path() + QStringLiteral("/dist/Application");
#ifdef Q_OS_LINUX
    expectedPath += QStringLiteral("/bin");
#endif
    expectedPath += QStringLiteral("/") + textFileName;
    const QFileInfo deployedFile(expectedPath);
    QVERIFY2(deployedFile.exists(),
             qPrintable("The general text file is not copied into the target dir "
                        "to the expected path " + expectedPath + ". "
                        "The dist dir content: [" + distDirListing()
                        + "]. The cqtdeployer output (the full output is stored in "
                        "test_report.txt.noQtGeneralFile):\n" + output));
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:

// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(QifwTest)
