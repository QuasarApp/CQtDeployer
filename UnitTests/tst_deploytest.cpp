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

#include <QByteArray>
#include <QDir>
#include <thread>
// add necessary includes here

class deploytest : public QObject
{
    Q_OBJECT

public:
    deploytest();
    /**
     * @brief generateLib
     * @param paath
     * @return size of lib
     */
    int generateLib(const QString& paath);
    void deleteLib(const QString& paath);

    ~deploytest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testDeployUtils();
    void testDeployTarget();
    void testTranslations();
    void testStrip();
    void testDeploy();
    void testMSVC();
};

deploytest::deploytest(){}

int deploytest::generateLib(const QString &paath)
{
    QDir dir;
    dir.mkpath(QFileInfo(paath).absolutePath());
    QFile testLib (paath);

    int size = 0;

    if (testLib.open(QIODevice::ReadWrite| QIODevice::Truncate)) {
        QFile resData(":/debugLib");
        if (resData.open(QIODevice::ReadOnly)) {
            QByteArray tempData = resData.readAll();
            size = tempData.size();
            testLib.write(tempData.data(), tempData.size());
            resData.close();
        }

        testLib.close();
    }

    return size;
}

void deploytest::deleteLib(const QString &paath)
{
    QFileInfo info(paath);
    if (info.isDir()) {
        QFile::remove(info.absoluteFilePath());
    } else {
        QDir qt(info.absoluteFilePath());
        qt.removeRecursively();
    }
}

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

void deploytest::testDeployTarget() {

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

void deploytest::testStrip() {

#ifdef Q_OS_WIN
    Deploy *deploy = new Deploy();
    QVERIFY(deploy->strip("./test/binTargetDir/debugLib.so"));
    delete deploy;
#else
    //for one lib
    qint64 sizeBefor = generateLib("./test/binTargetDir/debugLib.so");
    qint64 sizeAfter = 0;

    Deploy *deploy = new Deploy();
    QVERIFY(deploy->strip("./test/binTargetDir/debugLib.so"));

    QFile testLib ("./test/binTargetDir/debugLib.so");
    if (testLib.open(QIODevice::ReadOnly)) {
        sizeAfter = testLib.size();
        testLib.close();
    }

    deleteLib("./test/binTargetDir");
    delete deploy;

    QVERIFY(sizeBefor > sizeAfter);


    //for folder
    QStringList libList = {
        ("./test/binTargetDir/debugLib1.so"),
        ("./test/binTargetDir/debugLib2.so.1.2"),
        ("./test/binTargetDir/debugLib3.so.1"),
        ("./test/binTargetDir/debugLib4.so.1.0.0"),
        ("./test/binTargetDir/debugLib.dll"),
        ("./test/binTargetDir/debugLib1.dll")
    };
    QList<qint64> sizeBeforList = {};

    for (auto i: libList) {
        sizeBeforList.push_back(generateLib(i));
    }

    QList<qint64> sizeAfterList;

    deploy = new Deploy();
    QVERIFY(deploy->strip("./test/binTargetDir"));

    for(auto i: libList) {
        QFile testLib (i);
        if (testLib.open(QIODevice::ReadOnly)) {
            sizeAfterList.push_back(testLib.size());
            testLib.close();
        }
    }

    deleteLib("./test/binTargetDir");

    QVERIFY(sizeBeforList.size() == sizeAfterList.size());

    for (int i = 0; i < sizeAfterList.size(); ++i) {
        QVERIFY2(sizeBeforList[i] > sizeAfterList[i],
                 QString("index %0, lib: %1 size befor:%2, sizeAfter:%3").
                 arg(i).arg(libList[i]).arg(sizeBeforList[i]).arg(sizeAfterList[i]).
                 toLatin1());
    }
#endif
}

void deploytest::testDeploy() {
    QuasarAppUtils::Params::parseParams(0, nullptr);

    Deploy *deploy = new Deploy();
    QVERIFY(!deploy->appDir.isEmpty());
    delete deploy;
}

void deploytest::testMSVC() {
    QString testPath = "./Qt/5.11.2/msvc2017_64/bin/";
    QString testqmakepath = testPath +"qmake";

    QDir d;
    QDir oldDir("./Qt");
    oldDir.removeRecursively();
    QVERIFY(d.mkpath(testPath));

    QFile file(testqmakepath);

    QVERIFY(file.open(QIODevice::ReadWrite | QIODevice::Truncate));

    QVERIFY(file.write("test"));

    file.close();


    auto msvc = DeployUtils::getMSVC(testqmakepath);

    QVERIFY(msvc & MSVCVersion::MSVC_17);
    QVERIFY(msvc & MSVCVersion::MSVC_x64);

    QVERIFY(file.remove());

}

void deploytest::testTranslations() {
    QStringList trList = {
        ("./test/QtDir/translations/qtbase_ru.qm"),
        ("./test/QtDir/translations/qtbase_en.qm"),
        ("./test/QtDir/translations/qtmultimedia_en.qm"),
        ("./test/QtDir/translations/qtmultimedia_ru.qm"),
        ("./test/QtDir/translations/qtdeclarative_en.qm"),
        ("./test/QtDir/translations/qtdeclarative_ru.qm"),

    };

    QStringList res = {
        ("qtbase_ru.qm"),
        ("qtbase_en.qm"),
        ("qtmultimedia_en.qm"),
        ("qtmultimedia_ru.qm"),
    };

    QStringList libList = {
        (QFileInfo("./test/target/Qt5Core.so").absoluteFilePath()),
        (QFileInfo("./test/target/Qt5Nfc.so").absoluteFilePath()),
        (QFileInfo("./test/target/Qt5MultimediaWidgets.dll").absoluteFilePath()),

    };

    DeployUtils::qtDir = QFileInfo("./test/QtDir/").absoluteFilePath();


    Deploy *deploy = new Deploy();

    for (auto &&i: trList) {
        generateLib(i);
    }

    for (auto &&i: libList) {
        generateLib(i);
    }

    deploy->translationDir = QFileInfo("./test/QtDir/translations/").absoluteFilePath();
    deploy->targetDir = QFileInfo("./test/deploy/").absoluteFilePath();

    auto trans = DeployUtils::extractTranslation(libList);
    QVERIFY(trans.size() == 2);
    QVERIFY(deploy->copyTranslations(trans));

    QDir d("./test/deploy/translations") ;
    QVERIFY(d.exists());

    auto realList = d.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    QVERIFY(realList.size() == 4);

    for(auto i: realList) {

        QVERIFY(res.contains(i.fileName()));
    }

    deleteLib("./test/deploy");

}

QTEST_APPLESS_MAIN(deploytest)

#include "tst_deploytest.moc"
