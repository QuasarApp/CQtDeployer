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
 *  artifact contains data. The artifact is a single executable file on
 *  every platform: a self-extracting .run file on the Linux and the
 *  macOS hosts and an .exe file on the Windows host. On the macOS hosts
 *  the .app bundle created by the binarycreator is packed into the .run
 *  file by the second deploy command (issue #738, issue #884).
 */
bool installerIsNotEmpty(const QFileInfo &installer) {
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
    // ".run" suffix on the Linux and the macOS hosts and the ".exe"
    // suffix on the Windows host. The qifOut option below overrides the
    // default name, the suffix must match the platform (issue #738,
    // issue #884).
#ifdef Q_OS_WIN32
    return QString::fromLatin1(base) + QStringLiteral(".exe");
#else
    return QString::fromLatin1(base) + QStringLiteral(".run");
#endif
}

QFileInfo QifwTest::installerInfo(const char *base) const {
    // The installer artifact is a single executable file with the qifOut
    // name on every platform: a self-extracting .run file on the Linux
    // and the macOS hosts and an .exe file on the Windows host. On the
    // macOS hosts the .app bundle created by the binarycreator is packed
    // into the .run file and removed (issue #738, issue #884).
    const QString baseDir = _tmp.path() + "/dist/";
    return QFileInfo(baseDir + installerName(base));
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
    // The size of the installer artifact for the failure messages: the
    // artifact is a single file on every platform (issue #738, issue #884).
    const QFileInfo artifact = installerInfo();
    if (!artifact.exists()) {
        return QStringLiteral("<missing>");
    }
    return QString::number(artifact.size());
}

bool QifwTest::waitForInstaller(int timeoutMs, const char *base) {
    // The installer artifact is polled until it exists and is not empty
    // instead of being checked once right after the process exit: on the
    // macOS hosts the .run file is written by the second deploy command
    // (the packing of the .app bundle into the self-extracting archive),
    // and on the Windows and the Linux hosts the file artifact is complete
    // when the process exits, the first poll returns and the wait is a
    // no-op (issue #738, issue #884).
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

#ifdef Q_OS_MACOS
    // The macOS artifact is the self-extracting .run file (issue #884):
    // a POSIX shell header (the shebang, the comment, the unpack/launch
    // script) followed by the gzip-compressed tar archive of the .app
    // bundle. The bundle itself is removed after the packing into the
    // .run file.
    QFile runFile(installer.filePath());
    QVERIFY2(runFile.open(QIODevice::ReadOnly),
             "Failed to open the .run installer file for the structure check");
    const QByteArray data = runFile.readAll();
    runFile.close();
    const QByteArray bundleName = installer.fileName().toUtf8() + ".app";
    QVERIFY2(data.startsWith("#!/bin/sh"),
             "The .run installer file does not start with the sh shebang");
    QVERIFY2(data.contains("open -W \"$tmpDir/" + bundleName + "\""),
             "The .run installer file does not start the installer bundle "
             "(the .app name is missing from the shell header)");
    QVERIFY2(QFile::permissions(installer.filePath()).testFlag(QFile::ExeOwner),
             "The .run installer file is not executable");
    QVERIFY2(!QFileInfo(installer.filePath() + ".app").exists(),
             qPrintable("The .app bundle of the installer is not removed after the "
                        "packing into the .run file. The dist dir content: ["
                        + distDirListing() + "]"));
#endif
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
