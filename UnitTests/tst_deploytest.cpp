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
#include <configparser.h>
#include <QCryptographicHash>
#include <distrostruct.h>
#include <pathutils.h>

#include <QMap>
#include <QByteArray>
#include <QDir>
#include <thread>
#include "libcreator.h"
#include "modules.h"
#include "qmlcreator.h"
#include "testutils.h"
// add necessary includes here

const QString TestBinDir = TEST_BIN_DIR;
const QString TestQtDir = QT_BASE_DIR;

class deploytest : public QObject
{
    Q_OBJECT

private:
    QSet<QString> qtFilesTree;


    bool runProcess(const QString& DistroPath,
                    const QString& filename,
                    const QString &qt = "");
    QStringList getFilesFromDir(const QString& dir);


    void runTestParams(const QStringList &list, QSet<QString> *tree = nullptr, bool noWarnings = false);

    void checkResults(const QSet<QString> &tree, bool noWarnings);
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

    // old tests (not valid)
    void testDeployTarget();
    void testStrip();
    void testExtractLib();
    void testDistroStruct();
    void testRelativeLink();
    void testCheckQt();

    void testQmlExtrct();
    void testSetTargetDir();

    //    void mainTests();

    // end old tests

    // tested flags : help, version
    void testHelp();

    // tested flags clear noOvervrite
    void testOverwrite();

    // tested flags binDir
    void testBinDir();

    // tested flags confFile
    void testConfFile();

    // tested flags qmlDir qmake
    void testQt();

    // tested clear force clear in clear mode
    void testClear();

    // tested flags ignore ignoreEnv
    void testIgnore();

    // tested flags libDir recursiveDepth
    void testLibDir();

    // tested flag extraPlugin
    void testExtraPlugins();

    // tested flag targetDir
    void testTargetDir();

    // tested flag noLibc deploySystem
    void testSystemLib();

    // tested flag qmlOut libOut trOut pluginOut binOut
    void testOutDirs();

    void testMSVC();

    void testEmptyParamsString();

    void testWebEngine();
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

deploytest::deploytest() {
    TestUtils utils;

    auto tempTree = utils.getTree(TestQtDir);

    tempTree += utils.getTree("/lib", 4);
    tempTree += utils.getTree("/usr/lib", 4);

    for (const QString &i: tempTree) {
        qtFilesTree.insert(QFileInfo(i).fileName());
    }

}

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
    ConfigParser *deploy = new ConfigParser(&file);

    QStringList targets;
    targets << "./test/bins/execTarget.exe";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file);
    targets << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file);
    targets << "./test/bins/execTarget.exe" << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file);
    targets << "./test/bns/execTarget.exe";
    QVERIFY(!deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file);
    targets << "./test/bins/";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file);
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

    QDir d;
    QDir oldDir("./Qt");
    oldDir.removeRecursively();
    QVERIFY(d.mkpath(testPath));


    auto msvc = DeployCore::getMSVC(testPath);

    QVERIFY(msvc & MSVCVersion::MSVC_17);
    QVERIFY(msvc & MSVCVersion::MSVC_x64);

    QDir dir("./Qt");
    dir.removeRecursively();


}

void deploytest::testEmptyParamsString() {
    TestUtils utils;

#ifdef WITH_ALL_TESTS

    QDir("./" + DISTRO_DIR).removeRecursively();

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/UnitTests.sh",
                                             "./" + DISTRO_DIR + "/bin/qt.conf",
                                             "./" + DISTRO_DIR + "/bin/UnitTests",
                                         });

    runTestParams({}, &comapareTree);


    comapareTree = utils.createTree({});

    runTestParams({"clear"}, &comapareTree);

    comapareTree = utils.createTree({
                                        "./" + DISTRO_DIR + "/UnitTests.sh",
                                        "./" + DISTRO_DIR + "/bin/qt.conf",
                                        "./" + DISTRO_DIR + "/bin/UnitTests",
                                    });


    runTestParams({"-bin", "./UnitTests"
                  "-targetDor", "./testDeployDir"}, &comapareTree);


    comapareTree = utils.createTree({});

    runTestParams({"clear", "-targetDor", "./testDeployDir"}, &comapareTree);

#endif
}

void deploytest::testWebEngine() {
#ifdef WITH_ALL_TESTS
    TestUtils utils;


#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "quicknanobrowser";
    QString qmake = TestQtDir + "bin/qmake";

#else
    QString bin = TestBinDir + "quicknanobrowser.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

#endif

    auto comapareTree = Modules::qtWebEngine();


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../quicknanobrowser"}, &comapareTree);

#endif
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

void deploytest::testDistroStruct() {
    DistroStruct distro;

    auto cases = QList<QPair<QString,QString>>{
        {"", "/"},
        {"/", "/"},
        {"/res","/../"},
        {"/res/","/../"},
        {"/res/type","/../../"},
        {"/res/type/","/../../"},
        {"res/type","/../../"},
        {"res/type/","/../../"},
        {"res//type/","/../../"},
        {"res////type/","/../../"},
        {"//res///type/////","/../../"},
        {"\\", "/"},
        {"\\res","/../"},
        {"\\res\\","/../"},
        {"\\res\\type","/../../"},
        {"\\res\\type\\","/../../"},
        {"res\\type","/../../"},
        {"res\\type\\","/../../"},
        {"res\\\\type\\","/../../"},
        {"res\\\\\\\\type\\","/../../"},
        {"\\\\res\\\\\\type\\\\\\\\\\","/../../"},
    };

    for (auto &i: cases) {
        QVERIFY(distro.getRelativePath(i.first) == i.second);
    }
}

void deploytest::testRelativeLink() {
    auto cases = QList<QList<QString>>{
        {"", "", "./"},
        {"/media", "/etc", "./../etc/"},
        {"/media///", "/etc///", "./../etc/"},
        {"/media/etc/usr", "/media/etc", "./../"},
        {"/media/etc", "/media/etc/usr", "./usr/"},

        {"C:/", "C:/", "./"},
        {"C:\\", "C:/", "./"},
        {"C:/", "C:\\", "./"},

        {"C:/media", "C:/etc", "./../etc/"},
        {"C:/media//\\", "C:/etc///", "./../etc/"},
        {"C:/media/etc/usr", "C:/media/etc", "./../"},
        {"C:/media\\etc", "C:/media/etc/usr", "./usr/"},
        {"C:/media/etc", "D:/media/etc/usr", "D:/media/etc/usr"},

    };

    for (auto &i: cases) {
        QVERIFY(PathUtils::getRelativeLink(i[0], i[1]) == i[2]);
    }

    for (int i = 1; i < cases.size() - 1; i++) {
        QVERIFY(PathUtils::isAbsalutPath(cases[i][0]));
        QVERIFY(!PathUtils::isAbsalutPath(cases[i][2]));
    }
}

void deploytest::testCheckQt() {

    Deploy *deployer = new Deploy();
    QuasarAppUtils::Params::parseParams({"-binDir", TestBinDir, "clear"});
    QVERIFY(deployer->prepare());


    auto cases = QList<QPair<QString, bool>>{
        {TestQtDir + "/", false},
        {TestQtDir + "", false},
        {TestQtDir + "/bin/file1", false},
        {TestQtDir + "/lib/file12", false},
        {TestQtDir + "/resurces/file13", false},
        {TestQtDir + "/libexec/f", false},
        {TestQtDir + "/mkspecs", false},
        {TestQtDir + "/qml", false},
        {TestQtDir + "/plugins", false},
        {TestQtDir + "/file", false},

        {TestQtDir + "\\", false},
        {TestQtDir + "", false},
        {TestQtDir + "\\bin\\file1", false},
        {TestQtDir + "\\lib\\file12", false},
        {TestQtDir + "\\resurces\\file13", false},
        {TestQtDir + "\\libexec\\f", false},
        {TestQtDir + "\\mkspecs", false},
        {TestQtDir + "\\qml", false},
        {TestQtDir + "\\plugins", false},
        {TestQtDir + "\\file", false},

    };

    for (auto &i: cases) {
        QVERIFY(DeployCore::isQtLib(i.first) == i.second);
    }
    delete deployer;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestQMLWidgets";
    QString qmake = TestQtDir + "bin/qmake";

#else
    QString bin = TestBinDir + "TestQMLWidgets.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";
#endif

    deployer = new Deploy();
    QuasarAppUtils::Params::parseParams({"-bin", bin, "clear" ,
                                         "-qmake", qmake,
                                         "-qmlDir", TestBinDir + "/../TestQMLWidgets"});
    QVERIFY(deployer->prepare());


    cases = QList<QPair<QString, bool>>{
        {TestQtDir + "/", false},
        {TestQtDir + "", false},
        {TestQtDir + "/bin/file1", true},
        {TestQtDir + "/lib/file12", true},
        {TestQtDir + "/resources/file13", true},
        {TestQtDir + "/libexec/f", true},
        {TestQtDir + "/mkspecs", false},
        {TestQtDir + "/qml", true},
        {TestQtDir + "/plugins", true},
        {TestQtDir + "/file", false},

        {TestQtDir + "\\", false},
        {TestQtDir + "", false},
        {TestQtDir + "\\bin\\file1", true},
        {TestQtDir + "\\lib\\file12", true},
        {TestQtDir + "\\resources\\file13", true},
        {TestQtDir + "\\libexec\\f", true},
        {TestQtDir + "\\mkspecs", false},
        {TestQtDir + "\\qml", true},
        {TestQtDir + "\\plugins", true},
        {TestQtDir + "\\file", false},

    };

    for (auto &i: cases) {
        QVERIFY(DeployCore::isQtLib(i.first) == i.second);
    }

    delete deployer;
}

void deploytest::testSetTargetDir() {

    FileManager file;
    ConfigParser  dep(&file);

    dep.setTargetDir();

    QVERIFY(dep.config()->targetDir == QFileInfo("./" + DISTRO_DIR + "").absoluteFilePath());
    dep.setTargetDir("./ff");
    QVERIFY(dep.config()->targetDir == QFileInfo("./ff").absoluteFilePath());

    QStringList argv = {"-targetDir", "./" + DISTRO_DIR + "2"};

    QuasarAppUtils::Params::parseParams(argv);

    dep.setTargetDir();
    QVERIFY(dep.config()->targetDir == QFileInfo("./" + DISTRO_DIR + "2").absoluteFilePath());
    dep.setTargetDir("./ff");
    QVERIFY(dep.config()->targetDir == QFileInfo("./" + DISTRO_DIR + "2").absoluteFilePath());

}

void deploytest::runTestParams(const QStringList &list, QSet<QString>* tree, bool noWarnings) {

    QuasarAppUtils::Params::parseParams(list);

    Deploy deploy;
    QVERIFY(deploy.run() == 0);

    if (tree) {
        checkResults(*tree, noWarnings);
    }

#ifdef WITH_SNAP
#ifdef Q_OS_UNIX
    if (QFileInfo::exists("/snap/cqtdeployer/current/cqtdeployer.sh") && tree) {

        TestUtils utils;

        auto targetDir = DeployCore::_config->targetDir;
        QuasarAppUtils::Params::parseParams(QStringList{"clear",
                                                        "-targetDir", targetDir,
                                                        });

        Deploy deployClear;
        QVERIFY(deployClear.run() == 0);


        auto resultTree = utils.getTree(DeployCore::_config->targetDir);

        QVERIFY(!resultTree.size());

        QProcess cqtdeployerProcess;
        cqtdeployerProcess.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
        cqtdeployerProcess.setProgram("cqtdeployer");
        cqtdeployerProcess.setArguments(list);

        cqtdeployerProcess.start();

        QVERIFY(cqtdeployerProcess.waitForStarted());
        QVERIFY(cqtdeployerProcess.waitForFinished(3000000));

        checkResults(*tree, noWarnings);
    }
#endif
#endif
}

void deploytest::checkResults(const QSet<QString> &tree, bool noWarnings) {
    TestUtils utils;

    QVERIFY(DeployCore::_config);
    QVERIFY(!DeployCore::_config->targetDir.isEmpty());

    auto resultTree = utils.getTree(DeployCore::_config->targetDir);

    auto comapre = utils.compareTree(resultTree, tree);

    if (comapre.size() != 0) {

        bool bug = false;
        QJsonObject comapreResult;

        for (auto i = comapre.begin(); i != comapre.end(); ++i) {

            if (i.value() == 1) {
                comapreResult[ i.key()] = "Added unnecessary file";
                qCritical() << "added unnecessary file : " + i.key();
                bug = true;
            } else if (qtFilesTree.contains(QFileInfo(i.key()).fileName())) {
                comapreResult[ i.key()] = "Missing";
                qCritical() << "Missing file : " + i.key();
                bug = true;
            } else if (noWarnings)  {
                comapreResult[ i.key()] = " not exits in qt Dir";

                qCritical() << "File : " + i.key() + " not exits in qt Dir";
                bug = true;
            } else {
                comapreResult[ i.key()] = " not exits in qt Dir";
                qWarning() << "File : " + i.key() + " not exits in qt Dir";
            }
        }

        if (!bug) {
            return;
        }

        QJsonObject obj;
        for (const auto &i : resultTree) {
            obj[i];
        }

        QJsonDocument doc(obj);

        QFile lasttree("./LastTree.json");
        lasttree.open(QIODevice::WriteOnly| QIODevice::Truncate);

        lasttree.write(doc.toJson());
        lasttree.close();

        lasttree.setFileName("./CompareTree.json");
        lasttree.open(QIODevice::WriteOnly| QIODevice::Truncate);

        lasttree.write(QJsonDocument(comapreResult).toJson());
        lasttree.close();

        QVERIFY2(false, "runTestParams fail");


    }

}

void deploytest::testHelp() {
    runTestParams({"h"});
    runTestParams({"v"});
    auto help = DeployCore::help();

    for (auto &key: DeployCore::helpKeys()) {
        QVERIFY(help.contains(key));
    }

}

void deploytest::testOverwrite() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QFile f("./" + DISTRO_DIR + "/bin/TestOnlyC");
    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "/bin/TestOnlyC",
     "./" + DISTRO_DIR + "/bin/qt.conf",
     "./" + DISTRO_DIR + "/TestOnlyC.sh"});
    QString bin = TestBinDir + "TestOnlyC";

#else
    QFile f("./" + DISTRO_DIR + "/TestOnlyC.exe");
    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "/TestOnlyC.exe",
     "./" + DISTRO_DIR + "/qt.conf"});
    QString bin = TestBinDir + "TestOnlyC.exe";

#endif

    runTestParams({"-bin", bin, "force-clear", "noOverwrite"}, &comapareTree);

    QVERIFY(f.open(QIODevice::ReadOnly));
    auto hashBefor = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);
    f.close();

    QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Append));
    f.write(QByteArray(10, '1'));
    f.close();

    QVERIFY(f.open(QIODevice::ReadOnly));
    auto hashAfter = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);
    f.close();

    QVERIFY(hashAfter != hashBefor);

    runTestParams({"-bin", bin, "noOverwrite"}, &comapareTree);

    QVERIFY(f.open(QIODevice::ReadOnly));
    hashAfter = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);
    f.close();

    QVERIFY(hashAfter != hashBefor);


    runTestParams({"-bin", bin}, &comapareTree);

    QVERIFY(f.open(QIODevice::ReadOnly));
    hashAfter = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5);
    f.close();

    QVERIFY(hashAfter == hashBefor);

}

void deploytest::testBinDir() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "/bin/TestOnlyC",
     "./" + DISTRO_DIR + "/bin/qt.conf",
     "./" + DISTRO_DIR + "/bin/QtWidgetsProject",
     "./" + DISTRO_DIR + "/bin/TestQMLWidgets",
     "./" + DISTRO_DIR + "/TestOnlyC.sh",
     "./" + DISTRO_DIR + "/QtWidgetsProject.sh",
     "./" + DISTRO_DIR + "/TestQMLWidgets.sh"});
#else
    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "/TestOnlyC.exe",
     "./" + DISTRO_DIR + "/QtWidgetsProject.exe",
     "./" + DISTRO_DIR + "/TestQMLWidgets.exe",
     "./" + DISTRO_DIR + "/qt.conf"});
#endif

#ifdef WITH_ALL_TESTS
    comapareTree += utils.createTree(
    {"./" + DISTRO_DIR + "/bin/quicknanobrowser",
     "./" + DISTRO_DIR + "/quicknanobrowser.sh"});
#endif


    runTestParams({"-binDir", TestBinDir, "clear"}, &comapareTree);
}

void deploytest::testConfFile() {
    TestUtils utils;

    QFile::remove(TestBinDir + "/TestConf.json");
    QFile::remove(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");

#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "/bin/TestOnlyC",
     "./" + DISTRO_DIR + "/bin/qt.conf",
     "./" + DISTRO_DIR + "/bin/QtWidgetsProject",
     "./" + DISTRO_DIR + "/bin/TestQMLWidgets",
     "./" + DISTRO_DIR + "/TestOnlyC.sh",
     "./" + DISTRO_DIR + "/QtWidgetsProject.sh",
     "./" + DISTRO_DIR + "/TestQMLWidgets.sh"});
#else
    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "/TestOnlyC.exe",
     "./" + DISTRO_DIR + "/qt.conf",
     "./" + DISTRO_DIR + "/QtWidgetsProject.exe",
     "./" + DISTRO_DIR + "/TestQMLWidgets.exe"});
#endif

#ifdef WITH_ALL_TESTS
    comapareTree += utils.createTree(
    {"./" + DISTRO_DIR + "/bin/quicknanobrowser",
     "./" + DISTRO_DIR + "/quicknanobrowser.sh"});
#endif

    runTestParams({"-bin", TestBinDir, "clear" ,
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);


    QVERIFY(QFile::exists(TestBinDir + "/TestConf.json"));
    QFile::remove(TestBinDir + "/TestConf.json");

#ifdef Q_OS_UNIX
    runTestParams({"-bin", TestBinDir + "TestOnlyC," + TestBinDir + "QtWidgetsProject," + TestBinDir + "TestQMLWidgets",
                   "clear",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#else
    runTestParams({"-bin", TestBinDir + "TestOnlyC.exe," + TestBinDir + "QtWidgetsProject.exe," + TestBinDir + "TestQMLWidgets.exe",
                   "clear" , "-libDir", "L:/never/absalut/path",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#endif

    QFile confFile(TestBinDir + "/TestConf.json");
    QVERIFY(confFile.open(QIODevice::ReadOnly));

    auto data = confFile.readAll();
    confFile.close();

    QJsonDocument doc;
    doc = doc.fromJson(data);
    QVERIFY(!doc.isNull());

#ifdef Q_OS_UNIX

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("./TestOnlyC"));
    QVERIFY(data.contains("./QtWidgetsProject"));
    QVERIFY(data.contains("./TestQMLWidgets"));
//    QVERIFY(data.contains("\"libDir\": \"/never/absalut/path/\""));

    QVERIFY(data.contains("\"clear\": true"));

    data.insert(data.size() - 2, QString(",\"libDir\": \"/never/absalut/path/\"").toLatin1());

    QVERIFY(confFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    confFile.write(data);
    confFile.close();

    runTestParams({"-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#else

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("./TestOnlyC.exe"));
    QVERIFY(data.contains("./QtWidgetsProject.exe"));
    QVERIFY(data.contains("./TestQMLWidgets.exe"));
    QVERIFY(data.contains("\"libDir\": \"L:/never/absalut/path\""));

    QVERIFY(data.contains("\"clear\": true"));

    runTestParams({"-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);

#endif

    QVERIFY(QuasarAppUtils::Params::isEndable("clear"));
    QVERIFY(QuasarAppUtils::Params::isEndable("bin"));
    QVERIFY(QuasarAppUtils::Params::isEndable("libDir"));
#ifdef Q_OS_UNIX
    QVERIFY(QuasarAppUtils::Params::getStrArg("libDir") == "/never/absalut/path/");
#else
    QVERIFY(QuasarAppUtils::Params::getStrArg("libDir") == "L:/never/absalut/path");
#endif
    QFile::remove(TestBinDir + "/TestConf.json");


#ifdef Q_OS_UNIX
    runTestParams({"-bin", TestBinDir + "TestOnlyC," + TestBinDir + "QtWidgetsProject," + TestBinDir + "TestQMLWidgets",
                   "clear" ,
                   "-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);
#else
    runTestParams({"-bin", TestBinDir + "TestOnlyC.exe," + TestBinDir + "QtWidgetsProject.exe," + TestBinDir + "TestQMLWidgets.exe",
                   "clear" ,
                   "-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);
#endif

    confFile.setFileName(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");
    QVERIFY(confFile.open(QIODevice::ReadOnly));

    data = confFile.readAll();
    confFile.close();

    doc = doc.fromJson(data);
    QVERIFY(!doc.isNull());

#ifdef Q_OS_UNIX

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("./../../../../../build/TestOnlyC"));
    QVERIFY(data.contains("./../../../../../build/QtWidgetsProject"));
    QVERIFY(data.contains("./../../../../../build/TestQMLWidgets"));

    QVERIFY(data.contains("\"clear\": true"));

#else

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("./../../../../../build/TestOnlyC.exe"));
    QVERIFY(data.contains("./../../../../../build/QtWidgetsProject.exe"));
    QVERIFY(data.contains("./../../../../../build/TestQMLWidgets.exe"));

    QVERIFY(data.contains("\"clear\": true"));

#endif
    runTestParams({"-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);

    QVERIFY(QuasarAppUtils::Params::isEndable("clear"));
    QVERIFY(QuasarAppUtils::Params::isEndable("bin"));

    QFile::remove(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");

}

void deploytest::testQt() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestQMLWidgets";
    QString qmake = TestQtDir + "bin/qmake";

#else
    QString bin = TestBinDir + "TestQMLWidgets.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

#endif
    auto comapareTree = Modules::qmlLibs();



    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets"}, &comapareTree);

#ifdef Q_OS_UNIX
    bin = TestBinDir + "QtWidgetsProject";

#else
    bin = TestBinDir + "QtWidgetsProject.exe";

#endif
    comapareTree = Modules::qtLibs();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake}, &comapareTree);

    comapareTree = Modules::qtWithoutTr();


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake, "noTranslations"}, &comapareTree);

}

void deploytest::testClear() {
    TestUtils utils;


    auto compareTree = QSet<QString>{};

    runTestParams({"clear"}, &compareTree);

}

void deploytest::testIgnore() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "QtWidgetsProject";
    QString qmake = TestQtDir + "bin/qmake";
    auto comapareTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/QtWidgetsProject.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/QtWidgetsProject",
                });

#else
    QString bin = TestBinDir + "QtWidgetsProject.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

    auto comapareTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/QtWidgetsProject.exe",

                });

#endif


    if (!TestQtDir.contains("Qt5")) {

#ifdef Q_OS_UNIX
        QString bin = TestBinDir + "QtWidgetsProject";
        QString qmake = TestQtDir + "bin/qmake";
        comapareTree += utils.createTree(
        {
                        "./" + DISTRO_DIR + "/lib/libicudata.so",
                        "./" + DISTRO_DIR + "/lib/libicui18n.so",
                        "./" + DISTRO_DIR + "/lib/libicuuc.so"
                    });

#else
        QString bin = TestBinDir + "QtWidgetsProject.exe";
        QString qmake = TestQtDir + "bin/qmake.exe";

        comapareTree += utils.createTree(
        {
            "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
            "./" + DISTRO_DIR + "/libstdc++-6.dll",
            "./" + DISTRO_DIR + "/libwinpthread-1.dll"
        });

#endif
    }


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                  "-ignore", "Qt5"}, &comapareTree);



#ifdef Q_OS_UNIX
    comapareTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/QtWidgetsProject.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/QtWidgetsProject",
                });

    auto removeTree = utils.createTree({
                    "./" + DISTRO_DIR + "/lib/libQt5VirtualKeyboard.so",
                });

    auto removeTreePlugins = utils.createTree({
                  "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_hangul.so",
                  "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_openwnn.so",
                  "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_pinyin.so",
                  "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_tcime.so",
                  "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_thai.so",
                  "./" + DISTRO_DIR + "/plugins/platforminputcontexts/libqtvirtualkeyboardplugin.so",
                  "./" + DISTRO_DIR + "/lib/libQt5VirtualKeyboard.so",

                });

#else
    comapareTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/QtWidgetsProject.exe",
                });

    auto removeTree = utils.createTree({
                    "./" + DISTRO_DIR + "/Qt5VirtualKeyboard.dll",
                });

    auto removeTreePlugins = utils.createTree({
                  "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_hangul.dll",
                  "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_openwnn.dll",
                  "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_pinyin.dll",
                  "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_tcime.dll",
                  "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_thai.dll",
                  "./" + DISTRO_DIR + "/plugins/platforminputcontexts/qtvirtualkeyboardplugin.dll",
                  "./" + DISTRO_DIR + "/Qt5VirtualKeyboard.dll",

                });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                  "-ignoreEnv", TestQtDir + "/lib," + TestQtDir + "/bin" }, &comapareTree);



    comapareTree = Modules::qtLibs() - removeTree;

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                  "-ignore", "VirtualKeyboard"}, &comapareTree);

    comapareTree = Modules::qtLibs() - removeTreePlugins;

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                  "-ignore", "VirtualKeyboard,virtualkeyboard"}, &comapareTree);

}

void deploytest::testLibDir() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    QString extraPath = "/usr/lib,/lib";

    auto comapareTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/TestOnlyC"
                });

#else
    QString bin = TestBinDir + "TestOnlyC.exe";
    QString extraPath = TestQtDir;

    auto comapareTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",

                });

#endif




    runTestParams({"-bin", bin, "clear" ,
                   "-libDir", extraPath,
                  }, &comapareTree);


#ifdef Q_OS_UNIX
    comapareTree = utils.createTree(
    {
        "./" + DISTRO_DIR + "/TestOnlyC.sh",
        "./" + DISTRO_DIR + "/bin/qt.conf",
        "./" + DISTRO_DIR + "/bin/TestOnlyC",
        "./" + DISTRO_DIR + "/lib/libstdc++.so",
        "./" + DISTRO_DIR + "/lib/libgcc_s.so",
        "./" + DISTRO_DIR + "/lib/ld-linux-x86-64.so",
        "./" + DISTRO_DIR + "/lib/libc.so",
        "./" + DISTRO_DIR + "/lib/libm.so",
    });

    auto comapareTreeExtraLib = utils.createTree(
    {
        "./" + DISTRO_DIR + "/TestOnlyC.sh",
        "./" + DISTRO_DIR + "/bin/qt.conf",
        "./" + DISTRO_DIR + "/bin/TestOnlyC",
        "./" + DISTRO_DIR + "/lib/libstdc++.so",
        "./" + DISTRO_DIR + "/lib/libgcc_s.so"
    });

#else
    comapareTree = utils.createTree(
    {
        "./" + DISTRO_DIR + "/qt.conf",
        "./" + DISTRO_DIR + "/TestOnlyC.exe",
        "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
        "./" + DISTRO_DIR + "/libwinpthread-1.dll",
        "./" + DISTRO_DIR + "/libstdc++-6.dll",

    });

    auto comapareTreeExtraLib = utils.createTree(
    {
        "./" + DISTRO_DIR + "/qt.conf",
        "./" + DISTRO_DIR + "/TestOnlyC.exe",
        "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
        "./" + DISTRO_DIR + "/libstdc++-6.dll",

    });

#endif


    runTestParams({"-bin", bin, "clear" ,
                   "-libDir", extraPath,
                   "-recursiveDepth", "5"}, &comapareTree, true);

    runTestParams({"-bin", bin, "clear" ,
                   "-extraLib", "stdc,gcc"}, &comapareTreeExtraLib, true);


}

void deploytest::testExtraPlugins() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "QtWidgetsProject";
    QString extraPath = "/usr/lib";
    QString qmake = TestQtDir + "bin/qmake";

    auto pluginTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlodbc.so",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlpsql.so",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlite.so",
                    "./" + DISTRO_DIR + "/lib/libQt5Sql.so",

    });
#else
    QString bin = TestBinDir + "QtWidgetsProject.exe";
    QString extraPath = "/usr/lib";
    QString qmake = TestQtDir + "bin/qmake.exe";

    auto pluginTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlmysql.dll",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlodbc.dll",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlite.dll",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlpsql.dll",
                    "./" + DISTRO_DIR + "/Qt5Sql.dll",

    });
#endif

    auto comapareTree = Modules::qtLibs();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                  }, &comapareTree);


    comapareTree = comapareTree + pluginTree;


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-extraPlugin", "sqldrivers"}, &comapareTree);
}

void deploytest::testTargetDir() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "Z/bin/TestOnlyC",
     "./" + DISTRO_DIR + "Z/bin/qt.conf",
     "./" + DISTRO_DIR + "Z/TestOnlyC.sh"});
#else
    QString bin = TestBinDir + "TestOnlyC.exe";

    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "Z/TestOnlyC.exe",
     "./" + DISTRO_DIR + "Z/qt.conf"});
#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-targetDir", "./" + DISTRO_DIR + "Z"
                  }, &comapareTree);
}

void deploytest::testSystemLib() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    auto comapareTree = utils.createTree(
    {
         "./" + DISTRO_DIR + "/TestOnlyC.sh",
         "./" + DISTRO_DIR + "/bin/qt.conf",
         "./" + DISTRO_DIR + "/bin/TestOnlyC",
         "./" + DISTRO_DIR + "/lib/libgcc_s.so",
         "./" + DISTRO_DIR + "/lib/libstdc++.so"
    });

#else
    QString bin = TestBinDir + "TestOnlyC.exe";
    auto comapareTree = utils.createTree(
    {
        "./" + DISTRO_DIR + "/TestOnlyC.exe",
        "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
        "./" + DISTRO_DIR + "/libstdc++-6.dll",
        "./" + DISTRO_DIR + "/libwinpthread-1.dll",
        "./" + DISTRO_DIR + "/qt.conf"
    });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "deploySystem"
                  }, &comapareTree, true);

#ifdef Q_OS_UNIX

    QFile file("./" + DISTRO_DIR + "/TestOnlyC.sh");

    QVERIFY(file.open(QIODevice::ReadOnly));

    auto runScript = file.readAll();
    file.close();

    QVERIFY(!runScript.contains("export LD_PRELOAD="));

    comapareTree = utils.createTree(
    {
        "./" + DISTRO_DIR + "/TestOnlyC.sh",
        "./" + DISTRO_DIR + "/bin/qt.conf",
        "./" + DISTRO_DIR + "/bin/TestOnlyC",
        "./" + DISTRO_DIR + "/lib/libgcc_s.so",
        "./" + DISTRO_DIR + "/lib/ld-linux-x86-64.so",
        "./" + DISTRO_DIR + "/lib/libc.so",
        "./" + DISTRO_DIR + "/lib/libm.so",

        "./" + DISTRO_DIR + "/lib/libstdc++.so"
    });

    runTestParams({"-bin", bin, "clear" ,
                   "deploySystem-with-libc"
                  }, &comapareTree, true);

    file.setFileName("./" + DISTRO_DIR + "/TestOnlyC.sh");

    QVERIFY(file.open(QIODevice::ReadOnly));

    runScript = file.readAll();
    file.close();

    QVERIFY(runScript.contains("export LD_PRELOAD="));


#endif
}

void deploytest::testOutDirs() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestQMLWidgets";
    QString qmake = TestQtDir + "bin/qmake";

#else
    QString bin = TestBinDir + "TestQMLWidgets.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";


#endif
    auto comapareTree = Modules::outTestLibs();

    runTestParams({"-bin", bin, "clear" ,
                   "-binOut", "/lol",
                   "-libOut", "/lolLib",
                   "-trOut", "/lolTr",
                   "-pluginOut", "/p",
                   "-qmlOut", "/q",
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets"
                  }, &comapareTree);

    QFile file;

    file.setFileName("./" + DISTRO_DIR + "/lol/qt.conf");

    QVERIFY(file.open(QIODevice::ReadOnly));

    auto runScript = file.readAll();
    file.close();

    QVERIFY(runScript.contains("Prefix= ./../"));
    QVERIFY(runScript.contains("Libraries= ./lolLib/"));
    QVERIFY(runScript.contains("Plugins= ./p/"));
    QVERIFY(runScript.contains("Imports= ./q/"));
    QVERIFY(runScript.contains("Translations= ./lolTr/"));
    QVERIFY(runScript.contains("Qml2Imports= ./q/"));

#ifdef Q_OS_WIN


    runTestParams({"-bin", bin, "clear" ,
                   "-binOut", "/lol",
                   "-libOut", "/lolLib",
                   "-trOut", "/lolTr",
                   "-pluginOut", "/p",
                   "-qmlOut", "/q",
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets"
                  }, &comapareTree);



    file.setFileName( "./" + DISTRO_DIR + "/TestQMLWidgets.bat");

    QVERIFY(file.open(QIODevice::ReadOnly));

    runScript = file.readAll();
    file.close();

    QVERIFY(runScript.contains("SET BASE_DIR=%~dp0"));
    QVERIFY(runScript.contains("SET PATH=%BASE_DIR%\\\\lolLib\\;%PATH%"));
    QVERIFY(runScript.contains("start \"\" \"%BASE_DIR%\\\\lol\\TestQMLWidgets.exe\" %*"));


#endif

}

QTEST_APPLESS_MAIN(deploytest)

#include "tst_deploytest.moc"
