/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include <QtTest>
#include <quasarapp.h>
#include <deployutils.h>
#include <deploy.h>
#include <windependenciesscanner.h>

#include <QDir>
// add necessary includes here

class deploytest : public QObject
{
    Q_OBJECT

public:
    deploytest();
    ~deploytest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testDeployUtils();
    void testDeploy();
    void testDeployScaner();

};

deploytest::deploytest(){}

deploytest::~deploytest(){}

void deploytest::initTestCase() {
    QDir qt;
    qt.mkpath("./test/Qt/5.12/");
    qt.mkpath("./test/extraPath/");
    qt.mkpath("./test/extra/");
    qt.mkpath("./test/warning/");
    qt.mkpath("./test/bins/");

    QFile f( "./test/Qt/5.12/generalLib.so");
    if (f.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
        f.write("lib", 3);
        f.close();
    }

    f.setFileName("./test/extraPath/ExtraLib.so");
    if (f.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
        f.write("lib", 3);
        f.close();
    }

    f.setFileName("./test/extra/ExtraLib.so");
    if (f.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
        f.write("lib", 3);
        f.close();
    }

    f.setFileName("./test/warning/WarningLib.so");
    if (f.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
        f.write("lib", 3);
        f.close();
    }

    f.setFileName("./test/bins/execTarget.exe");
    if (f.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
        f.write("exec", 3);
        f.close();
    }

    f.setFileName("./test/bins/execTarget");
    if (f.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
        f.write("exec", 3);
        f.close();
    }
}

void deploytest::cleanupTestCase() {
    QDir qt("./test");
    qt.removeRecursively();

}

void deploytest::testDeployUtils() {
    QString qtDir = "./test/Qt/5.12/";
    QStringList extraPathes = QStringList() << QFileInfo("./test/extraPath/").absoluteFilePath();

    DeployUtils::qtDir = QFileInfo(qtDir).absoluteFilePath();
    DeployUtils::extraPaths = extraPathes;

    // qt Dir
    QVERIFY(DeployUtils::isQtLib("./test/Qt/5.12/myLib.so"));
    QVERIFY(!DeployUtils::isQtLib("/myQtDur/Qt/5.11/myLib.so"));
    QVERIFY(!DeployUtils::isQtLib("/mQtDur/Qt/5.12/myLib.so"));
    QVERIFY(DeployUtils::isQtLib("./test/Qt/5.12/myLib/testlibs/mylib.so"));

    // extra Dir
    QVERIFY(!DeployUtils::isExtraLib("./test/Qt/5.12/myLib.so"));
    QVERIFY(!DeployUtils::isExtraLib("/myQtDur/Qt/5.11/myLib.so"));
    QVERIFY(!DeployUtils::isExtraLib("/mQtDur/Qt/5.12/myLib.so"));
    QVERIFY(!DeployUtils::isExtraLib("./test/Qt/5.12/myLib/testlibs/mylib.so"));

    QVERIFY(DeployUtils::isExtraLib("./test/extraPath/Qt/5.12/myLib.so"));
    QVERIFY(DeployUtils::isExtraLib("./test/extraPath/Qt/5/myLib.so"));
    QVERIFY(DeployUtils::isExtraLib("./test/extraPath/myLib.so"));
    QVERIFY(DeployUtils::isExtraLib("./test/extraPath/Qt/5.12/myLib/testlibs/mylib.so"));

    //getLibPriority

    QVERIFY(DeployUtils::getLibPriority("./tst/Qt/5.12/generalLib.so") == 0);
    QVERIFY(DeployUtils::getLibPriority("./test/Qt/5.12/generalLib.so") == 3);
    QVERIFY(DeployUtils::getLibPriority("./test/extraPath/ExtraLib.so") == 2);
    QVERIFY(DeployUtils::getLibPriority("./test/extra/ExtraLib.so") == 1);


}

void deploytest::testDeploy() {

    Deploy *deploy = new Deploy();
    QStringList targets;
    targets << "./test/bins/execTarget.exe";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new Deploy();
    targets << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new Deploy();
    targets << "./test/bins/execTarget.exe" << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new Deploy();
    targets << "./test/bns/execTarget.exe";
    QVERIFY(!deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new Deploy();
    targets << "./test/bins/";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new Deploy();
    targets << "./test/bins/" << "./test/warning/";
    QVERIFY(deploy->setTargets(targets));

    delete deploy;
    targets.clear();


}

void deploytest::testDeployScaner() {

}

QTEST_APPLESS_MAIN(deploytest)

#include "tst_deploytest.moc"
