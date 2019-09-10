/*
 * Copyright (C) 2018-2019 QuasarApp.
 * Distributed under the lgplv3 software license, see the accompanying
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include <QtTest>
#include <quasarapp.h>
#include <deploycore.h>
#include <extracter.h>
#include <dependenciesscanner.h>
#include <qml.h>
#include <deploy.h>
#include <cqt.h>

#include <QMap>
#include <QByteArray>
#include <QDir>
#include <thread>
#include "libcreator.h"
#include "qmlcreator.h"
#include "testutils.h"
// add necessary includes here

class deploytest : public QObject
{
    Q_OBJECT

private:
    bool runProcess(const QString& DistroPath,
                    const QString& filename,
                    const QString &qt = "");
    QStringList getFilesFromDir(const QString& dir);


    void runTestParams(const QStringList &list, QSet<QString> *tree = nullptr);

public:
    deploytest();
    /**
     * @brief generateLib
     * @param paath
     * @return size of lib
     */
    int generateLib(const QString& paath);
    void deleteLib(const QString& paath);

    bool mainTestOnlyC();
    bool mainTestQMake();
    bool mainTestQML();
    bool testEnvIgnore();


    ~deploytest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    // old tests (not valid)
    void testDeployTarget();
    void testStrip();
    void testExtractLib();

    void testQmlExtrct();
    void testSetTargetDir();

    void mainTests();

    // end old tests

    void testHelp();
    void testMSVC();

};

bool deploytest::runProcess(const QString &DistroPath,
                            const QString &filename,
                            const QString& qt) {

    QProcess p;

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    if (qt.size()) {
        auto val = env.value("LD_LIBRARY_PATH","").remove(qt);
        env.insert("LD_LIBRARY_PATH", val);

        val = env.value("PATH","").remove(qt);
        env.insert("PATH", val);

        env.insert("QTDIR", "");
    } else {
        env.clear();
        env.insert("QTDIR", "");

    }
    p.setProcessEnvironment(env);

#ifdef Q_OS_UNIX
    p.setProgram(DistroPath + "/" + filename + ".sh");
#else
    p.setProgram(DistroPath + "/" + filename + ".exe");
#endif

    p.start();

    if (!p.waitForFinished(10000)) {
        return false;
    }

    QString str = p.readAll();
    if (p.exitCode()) {
        qCritical() << p.errorString();
    }

    if (p.exitCode()) {
        qWarning() << "exitCode == " <<  p.exitCode();
    }

    if (str.contains("failed to load component", Qt::CaseInsensitive)
            || str.contains("is not installed", Qt::CaseInsensitive) ||
            str.contains("error", Qt::CaseInsensitive)) {
        return false;
    }

    return p.exitCode() == 0;
}

QStringList deploytest::getFilesFromDir(const QString &path) {
        QDir dir(path);

        QStringList res;

        auto list = dir.entryInfoList(QDir::Dirs| QDir::Files| QDir::NoDotAndDotDot);

        for (auto &&subDir: list) {

            if (subDir.isFile()) {
                res.push_back(subDir.fileName());
            } else {
                res.append(getFilesFromDir(subDir.absoluteFilePath()));
            }
        }

        return res;
}

bool deploytest::testEnvIgnore()
{
#ifdef WITH_ALL_TESTS

    QFileInfo QtDir = QFileInfo(QT_BASE_DIR);

    if (!QtDir.isDir()) {
        return false;
    }

    int argc = 9;
    std::string qmakePath = (QtDir.absoluteFilePath() + "bin/qmake.exe").toStdString();
#ifdef Q_OS_WIN
    std::string qtPath = (QtDir.absoluteFilePath() + "bin/").toStdString();
#else
    std::string qtPath = (QtDir.absoluteFilePath() + "lib/").toStdString();
#endif
    const char *qmake = qmakePath.c_str();
    const char *qt = qtPath.c_str();
#ifdef Q_OS_WIN

    const char * argv[] = {"./",
                           "-bin", "./../../../tests/build/QtWidgetsProject.exe",
                           "-qmake", qmake,
                           "-ignoreEnv", qt,
                           "-targetDir", "./Distro"};
#else
    const char * argv[] = {"./",
                           "-bin", "./../../../tests/build/QtWidgetsProject",
                           "-qmake", qmake,
                           "-ignoreEnv", qt,

                           "-targetDir", "./Distro"};
#endif
    if (!QuasarAppUtils::Params::parseParams(argc, argv)) {
        return false;
    }

    Deploy deploy;

    if (deploy.run()) {
        return false;
    }

    if (!QFileInfo("./Distro").isDir()) {
        return false;
    }
#ifdef Q_OS_WIN
    QDir info("./Distro");

#else
    QDir info("./Distro/lib");
#endif

    for (auto &i :info.entryInfoList()) {
        if (i.fileName().contains("Qt")) {
            return false;
        }

    }
    if (!info.removeRecursively()) {
        return false;
    }

    return true;
#else
    return false;
#endif
}

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

void deploytest::testDeployTarget() {

    FileManager file;
    CQT *deploy = new CQT(&file);

    QStringList targets;
    targets << "./test/bins/execTarget.exe";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new CQT(&file);
    targets << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new CQT(&file);
    targets << "./test/bins/execTarget.exe" << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new CQT(&file);
    targets << "./test/bns/execTarget.exe";
    QVERIFY(!deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new CQT(&file);
    targets << "./test/bins/";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new CQT(&file);
    targets << "./test/bins/" << "./test/warning/";
    QVERIFY(deploy->setTargets(targets));

    delete deploy;
    targets.clear();
}

void deploytest::testStrip() {

#ifdef Q_OS_UNIX
    //for one lib
    qint64 sizeBefor = generateLib("./test/binTargetDir/debugLib.so");
    qint64 sizeAfter = 0;

    FileManager *deploy = new FileManager();
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

    deploy = new FileManager();
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

void deploytest::testExtractLib() {
    LibCreator creator("./");
    auto libs = creator.getLibs();
    auto deb = creator.getLibsDep();
    auto platforms = creator.getLibplatform();

    DependenciesScanner scaner;

    LibInfo info;

    for (auto &&lib : libs) {
        QVERIFY(scaner.fillLibInfo(info, lib));
        QVERIFY(info.getName() == QFileInfo(lib).fileName());
        QVERIFY(info.getPath() == QFileInfo(lib).absolutePath());
        QVERIFY(info.fullPath() == QFileInfo(lib).absoluteFilePath());
        QVERIFY(info.getPlatform() == platforms.value(lib));

        for (auto &dep : deb.value(lib)) {
            bool test = info.getDependncies().contains(dep.toUpper());
            QVERIFY(test);
        }

    }

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


    auto msvc = DeployCore::getMSVC(testqmakepath);

    QVERIFY(msvc & MSVCVersion::MSVC_17);
    QVERIFY(msvc & MSVCVersion::MSVC_x64);

    QVERIFY(file.remove());
    QDir dir("./Qt");
    dir.removeRecursively();


}

void deploytest::testQmlExtrct() {
    QmlCreator creator("./");
    auto imports = creator.getQmlImports();

    auto qmlFiles = creator.getCopyedQml();


    QML scaner("./");


    for (auto &&file : qmlFiles) {


        auto fileImports = scaner.extractImportsFromFile(file);

        for (auto &fil : imports.value(file)) {
            QVERIFY(fileImports.contains(fil, Qt::CaseInsensitive));
        }

    }
}

void deploytest::testSetTargetDir() {

    FileManager file;
    CQT  dep(&file);

    dep.setTargetDir();

    QVERIFY(dep.config()->targetDir == QFileInfo("./Distro").absoluteFilePath());
    dep.setTargetDir("./ff");
    QVERIFY(dep.config()->targetDir == QFileInfo("./ff").absoluteFilePath());

    int argc = 3;
    const char * argv[] = {"", "-targetDir", "./Distro2"};

    QuasarAppUtils::Params::parseParams(argc, argv);

    dep.setTargetDir();
    QVERIFY(dep.config()->targetDir == QFileInfo("./Distro2").absoluteFilePath());
    dep.setTargetDir("./ff");
    QVERIFY(dep.config()->targetDir == QFileInfo("./Distro2").absoluteFilePath());

}

void deploytest::mainTests() {
#ifdef WITH_ALL_TESTS
    QVERIFY(mainTestOnlyC());
    QVERIFY(mainTestQMake());
    QVERIFY(mainTestQML());
    QVERIFY(testEnvIgnore());


#endif
}

void deploytest::runTestParams(const QStringList &list, QSet<QString>* tree) {

    QuasarAppUtils::Params::parseParams(list);

    Deploy deploy;
    QVERIFY(deploy.run() == 0);

    if (tree) {
        TestUtils utils;

        QVERIFY(DeployCore::_config);
        QVERIFY(!DeployCore::_config->targetDir.isEmpty());

        auto resultTree = utils.getTree(DeployCore::_config->targetDir);
        auto comapre = utils.compareTree(resultTree, *tree);

        QVERIFY(comapre.size() == 0);

    }

}

void deploytest::testHelp() {
    runTestParams({"h"});
    runTestParams({"v"});

    for (auto &key: DeployCore::helpKeys()) {
        QVERIFY(DeployCore::help().contains(key));
    }

}

bool deploytest::mainTestOnlyC() {
#ifdef WITH_ALL_TESTS
    int argc = 5;
#ifdef Q_OS_WIN

    QFileInfo QtDir = QFileInfo(QT_BASE_DIR);

    if (!QtDir.isDir()) {
        return false;
    }
    argc += 2;

    std::string path = (QtDir.absoluteFilePath() + "bin/").toStdString();
    const char *string = path.c_str();

    const char * argv[] = {"./",
                           "-bin", "./../../../tests/build/TestOnlyC.exe",
                           "-targetDir", "./Distro",
                           "-libDir", string};
#else
    const char * argv[] = {"./",
                           "-bin", "./../../../tests/build/TestOnlyC",
                           "-targetDir", "./Distro"};
#endif

    if (!QuasarAppUtils::Params::parseParams(argc, argv)) {
        return false;
    }

    Deploy deploy;

    if (deploy.run()) {
        return false;
    }

    if (!QFileInfo("./Distro").isDir()) {
        return false;
    }

    QDir info("./Distro");

    bool run = runProcess("./Distro", "TestOnlyC");
    if (!info.removeRecursively()) {
        return false;
    }

    return run;
#else
    return false;

#endif
}

bool deploytest::mainTestQMake() {
#ifdef WITH_ALL_TESTS

    QFileInfo QtDir = QFileInfo(QT_BASE_DIR);

    if (!QtDir.isDir()) {
        return false;
    }

    int argc = 7;
#ifdef Q_OS_WIN

    std::string qmakePath = (QtDir.absoluteFilePath() + "bin/qmake.exe").toStdString();
    const char *qmake = qmakePath.c_str();
    const char * argv[] = {"./",
                           "-bin", "./../../../tests/build/QtWidgetsProject.exe",
                           "-qmake", qmake,
                           "-targetDir", "./Distro"};
#else
    const char * argv[] = {"./",
                           "-bin", "./../../../tests/build/QtWidgetsProject",
                           "-qmake", (QtDir.absoluteFilePath() + "/bin/qmake").toLatin1(),
                           "-targetDir", "./Distro"};
#endif
    if (!QuasarAppUtils::Params::parseParams(argc, argv)) {
        return false;
    }

    Deploy deploy;

    if (deploy.run()) {
        return false;
    }

    QDir info("./Distro");

    if (!QFileInfo("./Distro").isDir()) {
        return false;
    }

    bool run = runProcess("./Distro", "QtWidgetsProject", QtDir.absoluteFilePath());

    if (!info.removeRecursively()) {
        return false;
    }

    return run;
#else
    return false;
#endif



}

bool deploytest::mainTestQML() {

#ifdef WITH_ALL_TESTS

    QFileInfo QtDir = QFileInfo(QT_BASE_DIR);

    if (!QtDir.isDir()) {
        return false;
    }

    int argc = 9;
#ifdef Q_OS_WIN
    std::string qmakePath = (QtDir.absoluteFilePath() + "bin/qmake.exe").toStdString();
    const char *qmake = qmakePath.c_str();

    const char * argv[] = {"./",
                           "-bin", "./../../../tests/build/TestQMLWidgets.exe",
                           "-qmlDir", "./../../../tests/TestQMLWidgets",
                           "-qmake", qmake,
                           "-targetDir", "./Distro"};
#else
    const char * argv[] = {"./",
                           "-bin", "./../../../tests/build/TestQMLWidgets",
                           "-qmlDir", "./../../../tests/TestQMLWidgets",
                           "-qmake", (QtDir.absoluteFilePath() + "/bin/qmake").toLatin1(),
                           "-targetDir", "./Distro"};
#endif


    if (!QuasarAppUtils::Params::parseParams(argc, argv)) {
        return false;
    }

    Deploy deploy;

    if (deploy.run()) {
        return false;
    }

    QDir info("./Distro");

    bool run = runProcess("./Distro", "TestQMLWidgets", QtDir.absoluteFilePath());

    if (!info.removeRecursively()) {
        return false;
    }

    if (!run ) {
        return false;
    }

    if (!QFileInfo(QtDir).isDir()) {
        return false;
    }

    argc = 10;
#ifdef Q_OS_WIN

    const char * argv2[] = {"./",
                           "-bin", "./../../../tests/build/TestQMLWidgets.exe",
                           "-qmlDir", "./../../../tests/TestQMLWidgets",
                           "-qmake", qmake,
                           "-targetDir", "./Distro", "qmlExtern"};
#else
    const char * argv2[] = {"./",
                           "-bin", "./../../../tests/build/TestQMLWidgets",
                           "-qmlDir", "./../../../tests/TestQMLWidgets",
                           "-qmake", (QtDir.absoluteFilePath() + "/bin/qmake").toLatin1(),
                           "-targetDir", "./Distro", "qmlExtern"};
#endif


    if (!QuasarAppUtils::Params::parseParams(argc, argv2)) {
        return false;
    }

    Deploy deploy2;

    if (deploy.run()) {
        return false;
    }
    run = runProcess("./Distro", "TestQMLWidgets", QtDir.absoluteFilePath());

    if (!info.removeRecursively()) {
        return false;
    }

    return run;
#else
    return false;

#endif
}

QTEST_APPLESS_MAIN(deploytest)

#include "tst_deploytest.moc"
