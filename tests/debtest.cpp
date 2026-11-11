//
// Copyright (C) 2026 QuasarApp.
// Distributed under the lgplv3 software license, see the accompanying
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//

#include "debtest.h"

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
 *  DEB_TEST_CQT_DEPLOYER_EXE compile definition), so the tests cover the
 *  code under review and not the last stable release installed by the CI
 *  action. The cqtdeployer of the system PATH is used as a fallback.
 */
QString findDeployerExe() {
#ifdef DEB_TEST_CQT_DEPLOYER_EXE
    const QString buildTreeExe = QString::fromLatin1(DEB_TEST_CQT_DEPLOYER_EXE);
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

void DebTest::initTestCase() {
    _deployerExe = findDeployerExe();

#ifdef DEB_TEST_QMAKE
    _qmake = QString::fromLatin1(DEB_TEST_QMAKE);
#endif

    // The test executable itself is a deployed Qt application (the
    // deployTest target of the QuasarApp CMake CI targets), so it is a
    // valid target of the deb option: the deb package created for it is a
    // real deb package of a Qt application.
    _targetBinary = QCoreApplication::applicationFilePath();
    _targetLibDir = QCoreApplication::applicationDirPath();
}

void DebTest::cleanupTestCase() {
}

QString DebTest::debName(const char *base) const {
    // The Deb::deployTemplate() method of the Deploy library creates the
    // deb package in the target dir with the debOut value (the
    // <Name>.deb default), the debOut option below overrides the default
    // name.
    return QString::fromLatin1(base) + QStringLiteral(".deb");
}

QFileInfo DebTest::debInfo(const char *base) const {
    return QFileInfo(_tmp.path() + "/dist/" + debName(base));
}

QString DebTest::packageName() const {
    // The Package field of the DEBIAN/control file of the template is the
    // $NAME value of the package: for the default package it is the base
    // name of the first target (the iDistribution::getName() method of the
    // Deploy library). The license file of the Debian policy is stored in
    // the usr/share/doc/<package>/copyright file of the package, so the
    // name of the package is the same for the check of the package
    // content (issue #456).
    return QFileInfo(_targetBinary).baseName();
}

void DebTest::runDeployerProcess(const QStringList &args, const QString &runName,
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

QString DebTest::distDirListing() const {
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

void DebTest::optionTableCheck() {
    const auto options = DeployCore::avilableOptions();

    const QuasarAppUtils::OptionData *debOption = nullptr;
    const QuasarAppUtils::OptionData *debOutOption = nullptr;
    const QuasarAppUtils::OptionData *licenseOption = nullptr;
    for (const auto &option: options) {
        if (option.names() == (QStringList() << "deb" << "-deb")) {
            debOption = &option;
        }
        if (option.names() == (QStringList() << "-debOut")) {
            debOutOption = &option;
        }
        if (option.names() == (QStringList() << "-license")) {
            licenseOption = &option;
        }
    }

    // The deb option is registered in the options table.
    QVERIFY2(debOption != nullptr,
             "The deb option is not registered in the options table");
    if (debOption != nullptr) {
        QCOMPARE(debOption->names(), (QStringList() << "deb" << "-deb"));
        QVERIFY(!debOption->isDepricated());
    }

    // The debOut option is registered in the options table.
    QVERIFY2(debOutOption != nullptr,
             "The -debOut option is not registered in the options table");
    if (debOutOption != nullptr) {
        QCOMPARE(debOutOption->names(), (QStringList() << "-debOut"));
        QVERIFY(!debOutOption->isDepricated());
    }

    // The license option is registered in the options table: the license
    // file is inserted into the installer and the deb packages (issue
    // #456).
    QVERIFY2(licenseOption != nullptr,
             "The -license option is not registered in the options table");
    if (licenseOption != nullptr) {
        QCOMPARE(licenseOption->names(), (QStringList() << "-license"));
        QVERIFY(!licenseOption->isDepricated());
        QVERIFY(licenseOption->description().contains("deb", Qt::CaseInsensitive));
        QVERIFY(licenseOption->description().contains("copyright", Qt::CaseInsensitive));
        QVERIFY(licenseOption->example().contains("license"));
    }
}

bool DebTest::dpkgDebAvailable() const {
    return !QStandardPaths::findExecutable(QStringLiteral("dpkg-deb")).isEmpty();
}
void DebTest::debLicenseCheck() {
    // The deb option creates the deb packages with the dpkg-deb tool of
    // the Linux system (the Debian policy), so the package creation tests
    // are available on the Linux host platforms only.
#ifndef Q_OS_LINUX
    QSKIP("The deb option is available on the Linux host platforms only, "
          "the deb package test cannot run");
#else
    if (_deployerExe.isEmpty()) {
        QSKIP("The cqtdeployer executable of the build tree is not found, the deb package test cannot run");
    }

    if (!dpkgDebAvailable()) {
        QSKIP("The dpkg-deb tool of the Linux system is not found, "
              "the deb package test cannot run");
    }

    // The license file of the test with the unique marker: the marker is
    // checked in the copyright file of the created deb package (issue
    // #456).
    const QString marker = QStringLiteral("CQtDeployer DebTest License Agreement (issue #456)");
    const QString licenseFile = _tmp.path() + "/LICENSE.txt";
    {
        QFile file(licenseFile);
        QVERIFY2(file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text),
                 "Failed to create the license file");
        file.write(marker.toUtf8());
        file.close();
    }

    int exitCode = -1;
    QString output;
    QStringList args = {
        "clear",
        "-bin", _targetBinary,
        "-targetDir", _tmp.path() + "/dist",
        "-libDir", _targetLibDir,
        "-recursiveDepth", "5",
        "deb",
        "-license", licenseFile
    };
    args << "-debOut" << debName();
    if (!_qmake.isEmpty()) {
        args << "-qmake" << _qmake;
    }

    runDeployerProcess(args, QStringLiteral("license"), &exitCode, &output);
    QCOMPARE(exitCode, 0);

    const QFileInfo deb = debInfo();
    QVERIFY2(deb.isFile() && deb.size() > 0,
             qPrintable("The deb package is not created. The dist dir content: ["
                        + distDirListing()
                        + "]. The cqtdeployer output (the full output is stored in "
                          "test_report.txt.license):\n" + output));

    // The package information: the Package field of the DEBIAN/control
    // file is the name of the package.
    QProcess infoProc;
    infoProc.setProcessChannelMode(QProcess::MergedChannels);
    infoProc.start(QStringLiteral("dpkg-deb"),
                   {QStringLiteral("--info"), deb.filePath()});
    QVERIFY2(infoProc.waitForFinished(60 * 1000), "Failed to run the dpkg-deb tool");
    const QString infoOutput = QString::fromLocal8Bit(infoProc.readAll());
    QCOMPARE(infoProc.exitCode(), 0);
    QVERIFY2(infoOutput.contains("Package: " + packageName()),
             qPrintable("The Package field of the deb package is not the name of "
                        "the package (issue #456). The dpkg-deb --info output:\n"
                        + infoOutput));

    // The content of the package: the license file of the -license option
    // is stored in the standard location of the license text of the
    // Debian policy: the usr/share/doc/<package>/copyright file of the
    // package (issue #456).
    QProcess contentProc;
    contentProc.setProcessChannelMode(QProcess::MergedChannels);
    contentProc.start(QStringLiteral("dpkg-deb"),
                      {QStringLiteral("--contents"), deb.filePath()});
    QVERIFY2(contentProc.waitForFinished(60 * 1000), "Failed to run the dpkg-deb tool");
    const QString contentOutput = QString::fromLocal8Bit(contentProc.readAll());
    QCOMPARE(contentProc.exitCode(), 0);
    const QString copyrightPath = QStringLiteral("usr/share/doc/") + packageName()
        + QStringLiteral("/copyright");
    QVERIFY2(contentOutput.contains(copyrightPath),
             qPrintable("The copyright file of the license is not found in the "
                        "content of the deb package (issue #456). The expected "
                        "path: [" + copyrightPath
                        + "]. The dpkg-deb --contents output:\n" + contentOutput
                        + ". The cqtdeployer output (the full output is stored in "
                          "test_report.txt.license):\n" + output));

    // The content of the copyright file is the content of the license
    // file of the -license option (issue #456).
    const QString extractDir = _tmp.path() + "/extract";
    QProcess extractProc;
    extractProc.setProcessChannelMode(QProcess::MergedChannels);
    extractProc.start(QStringLiteral("dpkg-deb"),
                      {QStringLiteral("-x"), deb.filePath(), extractDir});
    QVERIFY2(extractProc.waitForFinished(60 * 1000), "Failed to run the dpkg-deb tool");
    QCOMPARE(extractProc.exitCode(), 0);

    const QFileInfo copyrightFile(extractDir + "/" + copyrightPath);
    QVERIFY2(copyrightFile.isFile(),
             qPrintable("The copyright file is not extracted from the deb package "
                        "(issue #456). The dpkg-deb --contents output:\n" + contentOutput));
    {
        QFile file(copyrightFile.filePath());
        QVERIFY2(file.open(QIODevice::ReadOnly), "Failed to open the copyright file");
        QCOMPARE(QString::fromUtf8(file.readAll()), marker);
    }
#endif
}

// Standard stand-alone QTest entry point, see the Qt test tutorial:

// https://doc.qt.io/qt-6/qttestlib-tutorial1-example.html
QTEST_GUILESS_MAIN(DebTest)