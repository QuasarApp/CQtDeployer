//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "appimagetest.h"

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
 *  APPIMAGE_TEST_CQT_DEPLOYER_EXE compile definition), so the tests cover
 *  the code under review and not the last stable release installed by the
 *  CI action. The cqtdeployer of the system PATH is used as a fallback.
 */
QString findDeployerExe() {
#ifdef APPIMAGE_TEST_CQT_DEPLOYER_EXE
    const QString buildTreeExe = QString::fromLatin1(APPIMAGE_TEST_CQT_DEPLOYER_EXE);
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

} // namespace

void AppImageTest::initTestCase() {
    _deployerExe = findDeployerExe();

#ifdef APPIMAGE_TEST_QMAKE
    _qmake = QString::fromLatin1(APPIMAGE_TEST_QMAKE);
#endif

    // The test executable itself is a deployed Qt application (the
    // deployTest target of the QuasarApp CMake CI targets), so it is a
    // valid target of the appimage option: the AppImage created for it is
    // a real AppImage of a Qt application.
    _targetBinary = QCoreApplication::applicationFilePath();
    _targetLibDir = QCoreApplication::applicationDirPath();
}

void AppImageTest::cleanupTestCase() {
}

QString AppImageTest::appimageName(const char *base) const {
    // The AppImage::deployTemplate() method of the Deploy library creates
    // the AppImage in the target dir with the appimageOut value (the
    // <Name>.AppImage default), the appimageOut option below overrides
    // the default name.
    return QString::fromLatin1(base) + QStringLiteral(".AppImage");
}

QFileInfo AppImageTest::appimageInfo(const char *base) const {
    return QFileInfo(_tmp.path() + "/dist/" + appimageName(base));
}

void AppImageTest::runDeployerProcess(const QStringList &args, const QString &runName,
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
    // cqtdeployer output of every run is stored in a separate file next to
    // the test report. The CI uploads all the test_report.txt* files of the
    // test directories, so the full output is available for the failed runs.
    QFile dump(QStringLiteral("test_report.txt.") + runName);
    if (dump.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream stream(&dump);
        stream << *output;
    }
}

QString AppImageTest::distDirListing() const {
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

bool AppImageTest::appImageIsValid(const QFileInfo &appImage) {
    // The AppImage is an ELF executable (the AppImage runtime with the
    // appended SquashFS content of the AppDir), so the created package is
    // a valid AppImage when it exists, is not empty and has the ELF magic
    // (0x7f 0x45 0x4c 0x46) in the first bytes.
    if (!appImage.isFile() || appImage.size() <= 4) {
        return false;
    }

    QFile f(appImage.filePath());
    if (!f.open(QIODevice::ReadOnly)) {
        return false;
    }

    return f.read(4) == QByteArray("\x7f" "ELF", 4);
}

void AppImageTest::optionTableCheck() {
    // The option table check is platform independent (the options are
    // registered on every platform, the appimage packages are created on
    // the Linux hosts only, issue #898).
    const auto options = DeployCore::avilableOptions();

    const QuasarAppUtils::OptionData *appimageOption = nullptr;
    for (const auto &option: options) {
        if (option.names().contains("appimage")) {
            appimageOption = &option;
        }
    }

    QVERIFY2(appimageOption != nullptr,
             "The appimage option is not registered in the options table");
    if (appimageOption != nullptr) {
        QCOMPARE(appimageOption->names(), (QStringList() << "appimage"));
        QVERIFY(!appimageOption->isDepricated());
    }
}

void AppImageTest::appimagePackageCheck() {
    // The appimage option creates the AppImage package of the target
    // (issue #898). The appimagetool is staged next to the cqtdeployer of
    // the build tree by the copyAppImage target, so the test covers the
    // bundled tool of the current build.
#ifndef Q_OS_LINUX
    QSKIP("The appimage option is available only on the Linux host platforms (issue #898)");
#else
    if (_deployerExe.isEmpty()) {
        QSKIP("The cqtdeployer executable of the build tree is not found, the appimage test cannot run");
    }

    int exitCode = -1;
    QString output;
    QStringList args = {
        "clear",
        "-bin", _targetBinary,
        "-targetDir", _tmp.path() + "/dist",
        "-libDir", _targetLibDir,
        "-recursiveDepth", "5",
        "appimage",
        "-appimageOut", appimageName()
    };
    if (!_qmake.isEmpty()) {
        args << "-qmake" << _qmake;
    }

    runDeployerProcess(args, QStringLiteral("appimage"), &exitCode, &output);
    QCOMPARE(exitCode, 0);

    const QFileInfo appImage = appimageInfo();
    QVERIFY2(appImageIsValid(appImage),
             qPrintable("The AppImage package is not created or is not a valid "
                        "AppImage. The dist dir content: [" + distDirListing()
                        + "]. The cqtdeployer output (the full output is stored in "
                        "test_report.txt.appimage):\n" + output));
#endif
}

void AppImageTest::noQtGeneralFileAppImageCheck() {
    // The appimage option with the noQt option: a target that cannot be
    // parsed as a binary of a known format (the ELF/PE/MACHO scanners
    // fail on it). The deployer must register such a target as a general
    // file, copy the file to the target dir and create the AppImage with
    // it. The AppDir of such a package contains no ELF binary, so the
    // appimagetool cannot detect the architecture from the content of the
    // AppDir and the AppImage distribution passes the architecture of
    // the current host with the ARCH environment variable (issue #898).
#ifndef Q_OS_LINUX
    QSKIP("The appimage option is available only on the Linux host platforms (issue #898)");
#else
    if (_deployerExe.isEmpty()) {
        QSKIP("The cqtdeployer executable of the build tree is not found, the appimage test cannot run");
    }

    const QString textFileName = QStringLiteral("clinerules");
    const QDir srcDir(_tmp.path() + "/src");
    QVERIFY2(srcDir.mkpath(QStringLiteral(".")), "Failed to create the source dir");

    const QString textFilePath = srcDir.absolutePath() + "/" + textFileName;
    {
        QFile f(textFilePath);
        QVERIFY2(f.open(QIODevice::WriteOnly | QIODevice::Text),
                 "Failed to create the general text file");
        f.write("A general text file used by the issue #898 regression test.\n");
    }

    int exitCode = -1;
    QString output;
    // The noQt and the appimage options are flags without the "-" prefix:
    // every option with the "-" prefix consumes the next token as its
    // value (the QuasarApp Params parser).
    runDeployerProcess({
        "clear",
        "-bin", textFilePath,
        "noQt",
        "appimage",
        "-targetDir", _tmp.path() + "/dist",
        "-appimageOut", appimageName("AppImageGeneralFile")
    }, QStringLiteral("noQtGeneralFile"), &exitCode, &output);
    QCOMPARE(exitCode, 0);

    // The smartMoveTargets() method of the ConfigParser copies every
    // target to <targetDir>/<package><binOutDir>/. The package is the
    // "Application" default package and the bin output dir is "/bin" on
    // the Linux hosts, so the general file target has an exact, known
    // location in the target dir.
    const QString expectedPath = _tmp.path() + QStringLiteral("/dist/Application/bin/") + textFileName;
    const QFileInfo deployedFile(expectedPath);
    QVERIFY2(deployedFile.exists(),
             qPrintable("The general text file is not copied into the target dir "
                        "to the expected path " + expectedPath + ". "
                        "The dist dir content: [" + distDirListing() + "]. "
                        "The cqtdeployer output (the full output is stored in "
                        "test_report.txt.noQtGeneralFile):\n" + output));

    const QFileInfo appImage = appimageInfo("AppImageGeneralFile");
    QVERIFY2(appImageIsValid(appImage),
             qPrintable("The AppImage package of the general file target is not "
                        "created or is not a valid AppImage. The dist dir content: ["
                        + distDirListing() + "]. The cqtdeployer output (the full "
                        "output is stored in test_report.txt.noQtGeneralFile):\n" + output));
#endif
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:

// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(AppImageTest)

