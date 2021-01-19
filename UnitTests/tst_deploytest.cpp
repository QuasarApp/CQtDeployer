/*
 * Copyright (C) 2018-2021 QuasarApp.
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
#include <dependencymap.h>
#include <packing.h>
#include <pluginsparser.h>
#include <zipcompresser.h>
#include <QStorageInfo>

#include <QMap>
#include <QByteArray>
#include <QDir>
#include <thread>
#include "libcreator.h"
#include "modules.h"
#include "qmlcreator.h"
#include "testutils.h"

// add necessary includes here


static const QString TestBinDir = TEST_BIN_DIR;
static const QString TestQtDir = QT_BASE_DIR;

class deploytest : public QObject
{
    Q_OBJECT

private:
    QSet<QString> filesTree;

    bool runProcess(const QString& DistroPath,
                    const QString& filename,
                    const QString &qt = "");
    QStringList getFilesFromDir(const QString& dir);

    void runTestParams(QStringList list,
                       QSet<QString> *tree = nullptr,
                       bool noWarnings = false,
                       bool onlySize = false,
                       exitCodes exitCode = exitCodes::Good);

    void checkResults(const QSet<QString> &tree,
                      bool noWarnings,
                      bool onlySize = false);

    void createTree(const QStringList& tree);

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
    void testRelativeLink();
    void testCheckQt();

    void testQmlExtrct();
    void testSetTargetDir();

    //    void mainTests();

    // end old tests

    // tested flags customScript

    void testZip();
    void costomScript();
    void testDistroStruct();

    // tested flags clear noOvervrite
    void testOverwrite();

    // tested flags binDir
    void testextraData();

    // tested flags qmlDir qmake
    void testQt();


    void testWebEngine();

    // tested flags confFile
    void testConfFile();

    // tested flags targetPackage
    void testPackages();

    // tested clear force clear in clear mode
    void testClear();

    // tested flags ignore
    void testIgnore();

    // tested flags ignore ignoreEnv
    void testIgnoreEnv();

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

    // qif flags
    void testQIF();
    void testQIFMulti();
    void testQIFCustom();

    // zip flags
    void testZIP();
    void testZIPMulti();

    // deb flags
    void testDEB();
    void testDEBMulti();
    void testDEBCustom();

    // qif and zip flags
    void testMultiPacking();

    // init flags
    void testInit();

    void testDependencyMap();

    void testQmlScaner();

    void testPrefix();
    // test configure empty packages
    void testallowEmptyPackages();
    // test skip empty packages
    void testEmptyPackages();

    void testRunScripts();
    void testGetDefaultTemplate();

    void customTest();
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

    for (const auto &subDir: qAsConst(list)) {

        if (subDir.isFile()) {
            res.push_back(subDir.fileName());
        } else {
            res.append(getFilesFromDir(subDir.absoluteFilePath()));
        }
    }

    return res;
}

deploytest::deploytest() {

    qputenv("QTEST_FUNCTION_TIMEOUT", "1800000");

    TestUtils utils;

    const QStringList pathList = QProcessEnvironment::systemEnvironment().
            value("PATH").split(DeployCore::getEnvSeparator());


    for (const auto& path: pathList) {
        filesTree += utils.getTree(path, 1);
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

    QDir("./" + DISTRO_DIR).removeRecursively();

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
    DependenciesScanner scan;
    Packing pac(&file);
    PluginsParser _pluginParser;

    ConfigParser *deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);

    QStringList targets;
    targets << "./test/bins/execTarget.exe";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bins/execTarget.exe" << "./test/bins/execTarget";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bns/execTarget.exe";
    QVERIFY(!deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
    targets << "./test/bins/";
    QVERIFY(deploy->setTargets(targets));
    delete deploy;
    targets.clear();

    deploy = new ConfigParser(&file, &_pluginParser, &scan, &pac);
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

    for (const auto & i: libList) {
        sizeBeforList.push_back(generateLib(i));
    }

    QList<qint64> sizeAfterList;

    deploy = new FileManager();
    QVERIFY(deploy->strip("./test/binTargetDir"));

    for(const auto &i: libList) {
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

    for (const auto &lib : libs) {
        QVERIFY(scaner.fillLibInfo(info, lib));
        QVERIFY(info.getName() == QFileInfo(lib).fileName());
        QVERIFY(info.getPath() == QFileInfo(lib).absolutePath());
        QVERIFY(info.fullPath() == QFileInfo(lib).absoluteFilePath());
        QVERIFY(info.getPlatform() == platforms.value(lib));

        for (const auto &dep : deb.value(lib)) {
            QString depName = dep;
            if (info.getPlatform() & Platform::Win) {
                depName = dep.toUpper();
            }

            bool test = info.getDependncies().contains(depName);
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
}

void deploytest::testWebEngine() {
#ifdef Q_OS_UNIX
    TestUtils utils;

    QString bin = TestBinDir + "quicknanobrowser";
    QString qmake = TestQtDir + "bin/qmake";

    auto comapareTree = TestModule.qtWebEngine();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../quicknanobrowser"}, &comapareTree);


    bin = TestBinDir + "webui";
    qmake = TestQtDir + "bin/qmake";

    comapareTree = TestModule.qtWebEngineWidgets();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake}, &comapareTree);


#endif
}

void deploytest::testQIF() {
    TestUtils utils;
#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestQMLWidgets";

    QString qmake = TestQtDir + "bin/qmake";
    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/InstallerTestQMLWidgets.run",
                                         });

#else
    QString bin = TestBinDir + "TestQMLWidgets.exe";

    QString qmake = TestQtDir + "bin/qmake.exe";
    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/InstallerTestQMLWidgets.exe",
                                         });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets",
                   "qif", "qifFromSystem",
                   "-qifStyle", "quasar",
                   "-qifBanner", TestBinDir + "/../../res/CQtDeployer_banner_web.png",
                   "-qifLogo", TestBinDir + "/../../res/CQtDeployer defaultIcon_web.png"}, &comapareTree, true);




}

void deploytest::testQIFMulti() {
    TestUtils utils;
#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestQMLWidgets";
    QString target1 = TestBinDir + "TestOnlyC";

    auto comapareTreeMulti = utils.createTree({
                                                  "./" + DISTRO_DIR + "/InstallerQtWidgetsProject.run",
                                              });

#else
    QString bin = TestBinDir + "TestQMLWidgets.exe";
    QString target1 = TestBinDir + "TestOnlyC.exe";

    auto comapareTreeMulti = utils.createTree({
                                                  "./" + DISTRO_DIR + "/InstallerQtWidgetsProject.exe",
                                              });

#endif

#ifdef Q_OS_UNIX
    QString target2 = TestBinDir + "TestQMLWidgets";
    QString target3 = TestBinDir + "QtWidgetsProject";

#else
    QString target2 = TestBinDir + "TestQMLWidgets.exe";
    QString target3 = TestBinDir + "QtWidgetsProject.exe";

#endif
    bin = target1;
    bin += "," + target2;
    bin += "," + target3;

    auto packageString = "/package1/;" + QFileInfo(target1).absoluteFilePath() + ",/package2/;" + QFileInfo(target2).absoluteFilePath();
    runTestParams({"-bin", bin, "force-clear",
                   "-binOut", "/lol",
                   "-libOut", "/lolLib",
                   "-trOut", "/lolTr",
                   "-pluginOut", "/p",
                   "-qmlOut", "/q",
                   "-qmlDir", "package2;" + TestBinDir + "/../TestQMLWidgets",
                   "-targetPackage", packageString,
                   "qif", "qifFromSystem"}, &comapareTreeMulti, true);
}

void deploytest::testQIFCustom() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestQMLWidgets";

    QString qmake = TestQtDir + "bin/qmake";

    auto comapareTreeCustom = utils.createTree({
                                                  "./" + DISTRO_DIR + "/Installerorg.qtproject.ifw.example.stylesheet.run",
                                              });
#else
    QString bin = TestBinDir + "TestQMLWidgets.exe";

    QString qmake = TestQtDir + "bin/qmake.exe";
    auto comapareTreeCustom = utils.createTree({
                                                  "./" + DISTRO_DIR + "/Installerorg.qtproject.ifw.example.stylesheet.exe",
                                              });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets",
                   "-qif", TestBinDir + "/../../UnitTests/testRes/QIFCustomTemplate",
                   "-name", "org.qtproject.ifw.example.stylesheet",
                   "qifFromSystem"}, &comapareTreeCustom, true);
}

void deploytest::testZIP() {

    TestUtils utils;

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/TestQMLWidgets.zip",
                                         });



#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestQMLWidgets";

    QString qmake = TestQtDir + "bin/qmake";

#else
    QString bin = TestBinDir + "TestQMLWidgets.exe";

    QString qmake = TestQtDir + "bin/qmake.exe";

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets",
                   "zip", "verbose"}, &comapareTree, true);


    // test clear for zip
    runTestParams({"clear", "verbose"}, nullptr, true);

}

void deploytest::testZIPMulti() {
    TestUtils utils;

    auto comapareTreeMulti = utils.createTree({
                                                  "./" + DISTRO_DIR + "/QtWidgetsProject.zip",
                                                  "./" + DISTRO_DIR + "/package1.zip",
                                                  "./" + DISTRO_DIR + "/package2.zip",

                                              });

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestQMLWidgets";
    QString target1 = TestBinDir + "TestOnlyC";

    QString target2 = TestBinDir + "TestQMLWidgets";
    QString target3 = TestBinDir + "QtWidgetsProject";

#else
    QString target2 = TestBinDir + "TestQMLWidgets.exe";
    QString target3 = TestBinDir + "QtWidgetsProject.exe";
    QString bin = TestBinDir + "TestQMLWidgets.exe";
    QString target1 = TestBinDir + "TestOnlyC.exe";


#endif
    bin = target1;
    bin += "," + target2;
    bin += "," + target3;

    auto packageString = "/package1/;" + QFileInfo(target1).absoluteFilePath() + ",/package2/;" + QFileInfo(target2).absoluteFilePath();
    runTestParams({"-bin", bin, "force-clear",
                   "-binOut", "/lol",
                   "-libOut", "/lolLib",
                   "-trOut", "/lolTr",
                   "-pluginOut", "/p",
                   "-qmlOut", "/q",
                   "-qmlDir", "package2;" + TestBinDir + "/../TestQMLWidgets",
                   "-targetPackage", packageString,
                   "zip"}, &comapareTreeMulti, true);
}

void deploytest::testDEB() {

#ifdef Q_OS_UNIX
    TestUtils utils;

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/TestQMLWidgets.deb",
                                         });


    QString bin = TestBinDir + "TestQMLWidgets";

    QString qmake = TestQtDir + "bin/qmake";

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets",
                   "deb", "verbose"}, &comapareTree, true);

    // test clear for deb
    runTestParams({"clear", "verbose"}, nullptr, true);

#endif

}

void deploytest::testDEBMulti() {
#ifdef Q_OS_UNIX
    TestUtils utils;

    auto comapareTreeMulti = utils.createTree({
                                                  "./" + DISTRO_DIR + "/QtWidgetsProject.deb",
                                                  "./" + DISTRO_DIR + "/package1.deb",
                                                  "./" + DISTRO_DIR + "/package2.deb",

                                              });

    QString bin = TestBinDir + "TestQMLWidgets";
    QString target1 = TestBinDir + "TestOnlyC";

    QString target2 = TestBinDir + "TestQMLWidgets";
    QString target3 = TestBinDir + "QtWidgetsProject";

    bin = target1;
    bin += "," + target2;
    bin += "," + target3;

    auto packageString = "/package1/;" + QFileInfo(target1).absoluteFilePath() + ",/package2/;" + QFileInfo(target2).absoluteFilePath();
    runTestParams({"-bin", bin, "force-clear",
                   "-binOut", "/lol",
                   "-libOut", "/lolLib",
                   "-trOut", "/lolTr",
                   "-pluginOut", "/p",
                   "-qmlOut", "/q",
                   "-qmlDir", "package2;" + TestBinDir + "/../TestQMLWidgets",
                   "-targetPackage", packageString,
                   "deb"}, &comapareTreeMulti, true);
#endif
}

void deploytest::testDEBCustom() {
#ifdef Q_OS_UNIX

    TestUtils utils;

    QString bin = TestBinDir + "TestQMLWidgets";

    QString qmake = TestQtDir + "bin/qmake";

    auto comapareTreeCustom = utils.createTree({
                                                  "./" + DISTRO_DIR + "/chrome.deb",
                                              });

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets",
                   "-deb", TestBinDir + "/../../UnitTests/testRes/DEBCustomTemplate",
                   "-name", "chrome"},
                  &comapareTreeCustom, true);
#endif
}

void deploytest::testMultiPacking() {
    TestUtils utils;

#ifdef Q_OS_UNIX

    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/TestQMLWidgets.zip",
                                             "./" + DISTRO_DIR + "/TestQMLWidgets.deb",
                                             "./" + DISTRO_DIR + "/InstallerTestQMLWidgets.run",
                                         });

    QString bin = TestBinDir + "TestQMLWidgets";

    QString qmake = TestQtDir + "bin/qmake";

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets",
                   "zip",
                   "qif", "qifFromSystem",
                   "deb",
                   "verbose"}, &comapareTree, true);

#else
    auto comapareTree = utils.createTree({
                                             "./" + DISTRO_DIR + "/TestQMLWidgets.zip",
                                             "./" + DISTRO_DIR + "/InstallerTestQMLWidgets.exe",
                                         });
    QString bin = TestBinDir + "TestQMLWidgets.exe";

    QString qmake = TestQtDir + "bin/qmake.exe";

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets",
                   "zip",
                   "qif", "qifFromSystem",
                   "verbose"}, &comapareTree, true);

#endif
}

void deploytest::testInit()
{

    TestUtils utils;

    runTestParams({"init"});

    QVERIFY(QFile(DEFAULT_COFIGURATION_FILE).remove());

    runTestParams({"-init", "multi"});


    QVERIFY(QFile(DEFAULT_COFIGURATION_FILE).remove());

    runTestParams({"-init", "single"});

    QVERIFY(QFile(DEFAULT_COFIGURATION_FILE).remove());

}

void deploytest::testDependencyMap() {
    DependencyMap dep1, dep2, dep3;

    QVERIFY(dep1.qtModules() == 0);
    QVERIFY(dep2.qtModules() == 0);
    QVERIFY(dep3.qtModules() == 0);

    QVERIFY(dep1.systemLibs().isEmpty());
    QVERIFY(dep2.systemLibs().isEmpty());
    QVERIFY(dep3.systemLibs().isEmpty());


    QVERIFY(dep1.neadedLibs().isEmpty());
    QVERIFY(dep2.neadedLibs().isEmpty());
    QVERIFY(dep3.neadedLibs().isEmpty());

    dep1.addModule(DeployCore::QtModule::QtGuiModule);

    QVERIFY(dep1.qtModules() == DeployCore::QtModule::QtGuiModule);
    dep1.addModule(DeployCore::QtModule::QtHelpModule);

    QVERIFY(dep1.qtModules() == (DeployCore::QtModule::QtGuiModule |
                                 DeployCore::QtModule::QtHelpModule));

    dep1.removeModule(DeployCore::QtModule::QtGuiModule);

    QVERIFY(dep1.qtModules() == DeployCore::QtModule::QtHelpModule);

    dep2.addModule(DeployCore::QtModule::QtGuiModule);

    dep1 += dep2;

    QVERIFY(dep1.qtModules() == (DeployCore::QtModule::QtGuiModule |
                                 DeployCore::QtModule::QtHelpModule));



}

void deploytest::testQmlScaner() {

    // qt5
    auto qmlRoot = QFileInfo(TestQtDir + "/qml").absoluteFilePath();
    QML *scaner = new QML(qmlRoot);
    auto imports = scaner->extractImportsFromFile(":/qmlFile.qml");

    scaner->scanQmlTree(qmlRoot);

    QSet<QString> results = {
        {qmlRoot + "/QtQuick.2/"},
        {qmlRoot + "/QtQuick/Controls.2/"},
        {qmlRoot + "/QtQuick/Controls.2/Material/"},
        {qmlRoot + "/QtQuick/Layouts/"},
    };

    QVERIFY(results.size() == imports.size());

    for (const auto &import: qAsConst(imports)) {
        auto path = scaner->getPathFromImport(import);
        QVERIFY(results.contains(path));
    }


    // qt6

    results = {
        {qmlRoot + "/QtQuick"},
        {qmlRoot + "/QtQuick/Controls"},
        {qmlRoot + "/QtQuick/Controls/Material"},
        {qmlRoot + "/QtQuick/Layouts"},
    };

    imports = scaner->extractImportsFromFile(":/qmlFileQt6.qml");

    QVERIFY(results.size() == imports.size());

    for (const auto &import: qAsConst(imports)) {
        auto path = scaner->getPathFromImport(import);
        QVERIFY(results.contains(path));
    }

}

void deploytest::testPrefix() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QFile f("./" + DISTRO_DIR + "/bin/TestOnlyC");
    auto comapareTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/package/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/package/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/package/bin/qt.conf"
                });
    QString target1 = TestBinDir + "TestOnlyC";

#else
    QFile f("./" + DISTRO_DIR + "/TestOnlyC.exe");
    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "/package/TestOnlyC.exe",
     "./" + DISTRO_DIR + "/package/TestOnlyC.bat",
     "./" + DISTRO_DIR + "/package/qt.conf"});
    QString target1 = TestBinDir + "TestOnlyC.exe";

#endif
    QString bin = target1;

    comapareTree = TestModule.replace(comapareTree, {{"package","prefix"}});

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "/package/;TestOn",
                   "-prefix", "package;prefix"}, &comapareTree);
}

void deploytest::testallowEmptyPackages() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif
    runTestParams({"-bin", bin, "force-clear",
                   "-prefix", "package;prefix"}, nullptr, false, false,
                  exitCodes::PrepareError);

    runTestParams({"-bin", bin, "force-clear",
                   "-prefix", "package;prefix",
                   "allowEmptyPackages"});
}

void deploytest::testEmptyPackages() {
    TestUtils utils;
    auto comapareTree = TestModule.onlyC();

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
#endif

    runTestParams({"-bin", bin, "force-clear",
                   "-prefix", "package;prefix"}, nullptr, false, false,
                  exitCodes::PrepareError);

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "/package/;NONE",
                   "-libDir", TestQtDir + "bin",
                   "-prefix", "package;prefix",
                   "allowEmptyPackages"}, &comapareTree);

#ifdef Q_OS_UNIX
    bin = TestBinDir + "QtWidgetsProject";

    comapareTree = utils.createTree(
                    {
                        "./" + DISTRO_DIR + "/QtWidgetsProject.zip",
                        "./" + DISTRO_DIR + "/QtWidgetsProject.deb",
                        "./" + DISTRO_DIR + "/InstallerQtWidgetsProject.run",
                    });

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "/package/;NONE",
                   "-prefix", "package;prefix",
                   "allowEmptyPackages",
                   "qif", "qifFromSystem",
                   "zip",
                   "deb"}, &comapareTree);
#else
    bin = TestBinDir + "QtWidgetsProject.exe";

    comapareTree = utils.createTree(
                    {
                        "./" + DISTRO_DIR + "/QtWidgetsProject.zip",
                        "./" + DISTRO_DIR + "/InstallerQtWidgetsProject.exe",
                    });

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "/package/;NONE",
                   "-prefix", "package;prefix",
                   "allowEmptyPackages",
                   "qif", "qifFromSystem",
                   "zip"
                  }, &comapareTree);
#endif
}

void deploytest::testRunScripts() {
    TestUtils utils;
    auto comapareTree = TestModule.onlyC();

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";
    QFile f(":/testResurces/testRes/customRunScript.sh");
    QVERIFY(f.open(QIODevice::ReadOnly));
    auto etalonData = f.readAll();
    f.close();

    runTestParams({"-bin", bin,
                   "force-clear",
                   "-libOut", "lib",
                   "-runScript", "TestOnlyC;:/testResurces/testRes/customRunScript.sh"}, &comapareTree);

    f.setFileName(DISTRO_DIR + "/TestOnlyC.sh");
    QVERIFY(f.open(QIODevice::ReadOnly));
    auto deployData = f.readAll();

    QVERIFY(deployData == etalonData);
#else
    QString bin = TestBinDir + "TestOnlyC.exe";
    QFile f(":/testResurces/testRes/customRunScript.sh");
    QVERIFY(f.open(QIODevice::ReadOnly));
    auto etalonData = f.readAll();
    f.close();

    runTestParams({"-bin", bin,
                   "force-clear",
                   "-libOut", "lib",
                  "-runScript", "TestOnlyC.exe;:/testResurces/testRes/customRunScript.sh"}, &comapareTree);

    f.setFileName(DISTRO_DIR + "/TestOnlyC.bat");
    QVERIFY(f.open(QIODevice::ReadOnly));
    auto deployData = f.readAll();

    QVERIFY(deployData == etalonData);
#endif

}

void deploytest::testGetDefaultTemplate() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "TestOnlyC";

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/defaultDEBTemplate/Application/DEBIAN/control",
                    "./" + DISTRO_DIR + "/defaultDEBTemplate/Application/DEBIAN/postinst",
                    "./" + DISTRO_DIR + "/defaultDEBTemplate/Application/DEBIAN/prerm",
                    "./" + DISTRO_DIR + "/defaultDEBTemplate/Application/opt/Application/icons/Icon.png",
                    "./" + DISTRO_DIR + "/defaultQIFWTemplate/config/config.xml",
                    "./" + DISTRO_DIR + "/defaultQIFWTemplate/config/controlScript.qs",
                    "./" + DISTRO_DIR + "/defaultQIFWTemplate/packages/Application/data/icons/Icon.png",
                    "./" + DISTRO_DIR + "/defaultQIFWTemplate/packages/Application/meta/installscript.qs",
                    "./" + DISTRO_DIR + "/defaultQIFWTemplate/packages/Application/meta/package.xml"
                });
    runTestParams(
                {"-bin", bin,
                 "force-clear",
                 "getDefaultTemplate",
                 "deb",
                 "qif"
                }, &comapareTree);
#else
    QString bin = TestBinDir + "TestOnlyC.exe";

    auto comapareTree = utils.createTree(
                {
                    "./" + DISTRO_DIR + "/defaultQIFWTemplate/config/config.xml",
                    "./" + DISTRO_DIR + "/defaultQIFWTemplate/config/controlScript.qs",
                    "./" + DISTRO_DIR + "/defaultQIFWTemplate/packages/Application/data/icons/Icon.png",
                    "./" + DISTRO_DIR + "/defaultQIFWTemplate/packages/Application/meta/installscript.qs",
                    "./" + DISTRO_DIR + "/defaultQIFWTemplate/packages/Application/meta/package.xml"
                });
    runTestParams(
                {"-bin", bin,
                 "force-clear",
                 "getDefaultTemplate",
                 "qif"
                }, &comapareTree);
#endif

}

void deploytest::customTest() {
//    runTestParams({"-confFile", "path",
//                   "qifFromSystem"});
}

void deploytest::testQmlExtrct() {
    QmlCreator creator("./");
    auto imports = creator.getQmlImports();

    auto qmlFiles = creator.getCopyedQml();


    QML scaner("./");


    for (const auto &file : qAsConst(qmlFiles)) {


        auto fileImports = scaner.extractImportsFromFile(file);

        for (const auto &fil : imports.value(file)) {
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
        {"res/type","../../"},
        {"res/type/","../../"},
        {"res//type/","../../"},
        {"res////type/","../../"},
        {"//res///type/////","/../../"},
        {"\\", "/"},
        {"\\res","/../"},
        {"\\res\\","/../"},
        {"\\res\\type","/../../"},
        {"\\res\\type\\","/../../"},
        {"res\\type","../../"},
        {"res\\type\\","../../"},
        {"res\\\\type\\","../../"},
        {"res\\\\\\\\type\\","../../"},
        {"\\\\res\\\\\\type\\\\\\\\\\","/../../"},
    };

    for (const auto &i: qAsConst(cases)) {
        if (distro.getRelativePath(i.first) != i.second)
            QVERIFY(false);
    }

    distro = DistroStruct();

    distro.setTrOutDir("/tr/");
    QVERIFY(distro.getTrOutDir() == "/tr/");


    distro.setTrOutDir("/tr");
    QVERIFY(distro.getTrOutDir() == "/tr/");

    distro.setTrOutDir("tr");
    QVERIFY(distro.getTrOutDir() == "/tr/");


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

    for (const auto &i: qAsConst(cases)) {
        if (PathUtils::getRelativeLink(i[0], i[1]) != i[2])
            QVERIFY(false);
    }

    for (int i = 1; i < cases.size() - 1; i++) {
        if (!PathUtils::isAbsalutPath(cases[i][0]))
            QVERIFY(false);
        if (PathUtils::isAbsalutPath(cases[i][2]))
            QVERIFY(false);

    }
}

void deploytest::testCheckQt() {

    Deploy *deployer = new Deploy();
    QuasarAppUtils::Params::parseParams({"-bin", TestBinDir, "clear",
                                         "noCheckRPATH", "noCheckPATH", "noQt"});
    QVERIFY(deployer->prepare());


    auto cases = QList<QPair<QString, QtMajorVersion>>{
        {TestQtDir + "/", QtMajorVersion::NoQt},
        {TestQtDir + "", QtMajorVersion::NoQt},
        {TestQtDir + "/bin/file1", QtMajorVersion::NoQt},
        {TestQtDir + "/lib/file12.so", QtMajorVersion::NoQt},
        {TestQtDir + "/resurces/file13.dll", QtMajorVersion::NoQt},
        {TestQtDir + "/libexec/f", QtMajorVersion::NoQt},
        {TestQtDir + "/mkspecs", QtMajorVersion::NoQt},
        {TestQtDir + "/qml", QtMajorVersion::NoQt},
        {TestQtDir + "/plugins", QtMajorVersion::NoQt},
        {TestQtDir + "/file", QtMajorVersion::NoQt},

        {TestQtDir + "\\", QtMajorVersion::NoQt},
        {TestQtDir + "", QtMajorVersion::NoQt},
        {TestQtDir + "\\bin\\file1", QtMajorVersion::NoQt},
        {TestQtDir + "\\lib\\file12", QtMajorVersion::NoQt},
        {TestQtDir + "\\resurces\\file13", QtMajorVersion::NoQt},
        {TestQtDir + "\\libexec\\f.so", QtMajorVersion::NoQt},
        {TestQtDir + "\\mkspecs.dll", QtMajorVersion::NoQt},
        {TestQtDir + "\\qml", QtMajorVersion::NoQt},
        {TestQtDir + "\\plugins", QtMajorVersion::NoQt},
        {TestQtDir + "\\file", QtMajorVersion::NoQt},

    };

    for (const auto &i: qAsConst(cases)) {
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


    cases = QList<QPair<QString, QtMajorVersion>>{
        {TestQtDir + "/", QtMajorVersion::NoQt},
        {TestQtDir + "", QtMajorVersion::NoQt},
        {TestQtDir + "/bin/file1", QtMajorVersion::NoQt},
        {TestQtDir + "/lib/file12", QtMajorVersion::NoQt},
        {TestQtDir + "/lib/file12", QtMajorVersion::NoQt},

        {TestQtDir + "/mkspecs", QtMajorVersion::NoQt},
        {TestQtDir + "/qml", QtMajorVersion::NoQt},
        {TestQtDir + "/plugins", QtMajorVersion::NoQt},
        {TestQtDir + "/file", QtMajorVersion::NoQt},

        {TestQtDir + "\\", QtMajorVersion::NoQt},
        {TestQtDir + "", QtMajorVersion::NoQt},
        {TestQtDir + "\\lib\\file12", QtMajorVersion::NoQt},
        {TestQtDir + "\\libexec\\fQt", QtMajorVersion::NoQt},
        {TestQtDir + "\\mkspecs", QtMajorVersion::NoQt},
        {TestQtDir + "\\qml", QtMajorVersion::NoQt},
        {TestQtDir + "\\plugins", QtMajorVersion::NoQt},
        {TestQtDir + "\\file", QtMajorVersion::NoQt},

        {TestQtDir + "/bin/file1Qt4.so", QtMajorVersion::Qt4},
        {TestQtDir + "/resources/Qt4file13.so", QtMajorVersion::Qt4},
        {TestQtDir + "/libexec/Qt4f.dll", QtMajorVersion::Qt4},
        {TestQtDir + "\\bin\\Qt4file1.dll", QtMajorVersion::Qt4},
        {TestQtDir + "\\resources\\fileQt413.dll", QtMajorVersion::Qt4},

        {TestQtDir + "/bin/file1Qt5.so", QtMajorVersion::Qt5},
        {TestQtDir + "/resources/Qt5file13.so", QtMajorVersion::Qt5},
        {TestQtDir + "/libexec/Qt5f.dll", QtMajorVersion::Qt5},
        {TestQtDir + "\\bin\\Qt5file1.dll", QtMajorVersion::Qt5},
        {TestQtDir + "\\resources\\fileQt513.dll", QtMajorVersion::Qt5},

        {TestQtDir + "/bin/file1Qt6.so", QtMajorVersion::Qt6},
        {TestQtDir + "/resources/Qt6file13.so", QtMajorVersion::Qt6},
        {TestQtDir + "/libexec/Qt6f.dll", QtMajorVersion::Qt6},
        {TestQtDir + "\\bin\\Qt6file1.dll", QtMajorVersion::Qt6},
        {TestQtDir + "\\resources\\fileQt613.dll", QtMajorVersion::Qt6},

    };

    for (const auto &i: qAsConst(cases)) {
        auto dexription = QString("The isQtLib(%0) function should be return %1").arg(
                    i.first).arg(i.second);
        QVERIFY2(DeployCore::isQtLib(i.first) == i.second, dexription.toLatin1().data());
    }

    delete deployer;
}

void deploytest::testSetTargetDir() {

    FileManager file;
    DependenciesScanner scan;
    Packing pac(&file);
    PluginsParser _plugins;
    ConfigParser  dep(&file, &_plugins, &scan, &pac);

    dep.setTargetDir();

    QVERIFY(dep.config()->getTargetDir() == QFileInfo("./" + DISTRO_DIR + "").absoluteFilePath());
    dep.setTargetDir("./ff");
    QVERIFY(dep.config()->getTargetDir() == QFileInfo("./ff").absoluteFilePath());

    QStringList argv = {"-targetDir", "./" + DISTRO_DIR + "2"};

    QuasarAppUtils::Params::parseParams(argv);

    dep.setTargetDir();
    QVERIFY(dep.config()->getTargetDir() == QFileInfo("./" + DISTRO_DIR + "2").absoluteFilePath());
    dep.setTargetDir("./ff");
    QVERIFY(dep.config()->getTargetDir() == QFileInfo("./" + DISTRO_DIR + "2").absoluteFilePath());

}

void deploytest::testZip() {
    TestUtils utils;

    ZipCompresser zip;
    auto befor = utils.getTree("./test");

    QVERIFY(zip.compress("./test", "./arr.zip"));
    QVERIFY(QDir("./test").removeRecursively());
    QVERIFY(zip.extract("./arr.zip", "./test"));

    auto after = utils.getTree("./test");

    QVERIFY(utils.compareTree(befor, after).size() == 0);


}

void deploytest::runTestParams(QStringList list,
                               QSet<QString>* tree,
                               bool noWarnings, bool onlySize,
                               exitCodes exitCode) {

    QuasarAppUtils::Params::parseParams(list);

    Deploy deploy;
    if (deploy.run() != exitCode)
        QVERIFY(false);

    if (tree) {
        checkResults(*tree, noWarnings, onlySize);
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

void deploytest::checkResults(const QSet<QString> &tree,
                              bool noWarnings,
                              bool onlySize) {
    TestUtils utils;

    QVERIFY(DeployCore::_config);
    QVERIFY(!DeployCore::_config->getTargetDir().isEmpty());

    auto resultTree = utils.getTree(DeployCore::_config->getTargetDir());

#ifdef Q_OS_WIN
    // Remove all API-MS-Win libs, because each OS Windows have a own bundle of this api libs.
    // See the https://github.com/QuasarApp/CQtDeployer/issues/481#issuecomment-755156875 post for more information.
    resultTree = TestModule.ignoreFilter(resultTree, "API-MS-Win");

#endif


    auto comapre = utils.compareTree(resultTree, tree);

    if (onlySize) {
        QVERIFY(resultTree.size() > tree.size());
        return;
    }

    if (comapre.size() != 0) {

        bool bug = false;
        QJsonObject comapreResult;

        for (auto i = comapre.begin(); i != comapre.end(); ++i) {

            if (i.value() == 1) {
                comapreResult[ i.key()] = "Added unnecessary file";
                qCritical() << "added unnecessary file : " + i.key();
                bug = true;
            } else if (filesTree.contains(QFileInfo(i.key()).fileName())) {
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
        for (const auto &i : qAsConst(resultTree)) {
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

void deploytest::createTree(const QStringList &tree) {
    for (const auto& dir : tree) {
        QDir().mkpath(dir);
    }
}

void deploytest::costomScript() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QFile f("./" + DISTRO_DIR + "/bin/TestOnlyC");
    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "/bin/TestOnlyC",
     "./" + DISTRO_DIR + "/bin/qt.conf",
     "./" + DISTRO_DIR + "/TestOnlyC.sh"});
    QString bin = TestBinDir + "TestOnlyC";
    QString scriptPath = "./" + DISTRO_DIR + "/TestOnlyC.sh";

#else
    QFile f("./" + DISTRO_DIR + "/TestOnlyC.exe");
    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "/TestOnlyC.exe",
     "./" + DISTRO_DIR + "/TestOnlyC.bat",
     "./" + DISTRO_DIR + "/qt.conf"});
    QString bin = TestBinDir + "TestOnlyC.exe";
    QString scriptPath = "./" + DISTRO_DIR + "/TestOnlyC.bat";


#endif

    runTestParams({"-bin", bin, "force-clear", "noOverwrite", "-libOut", "lib"}, &comapareTree);

    QFile script(scriptPath);
    QVERIFY(script.open(QIODevice::ReadOnly));
    auto scriptText = script.readAll();

    QVERIFY(!scriptText.contains("Begin Custom Script"));

    script.close();

    runTestParams({"-bin", bin, "force-clear", "noOverwrite",
                   "-libOut", "lib",
                   "-customScript", "echo 'this is test script'"}, &comapareTree);

    QVERIFY(script.open(QIODevice::ReadOnly));

    scriptText = script.readAll();

    QVERIFY(scriptText.contains("Begin Custom Script"));
    QVERIFY(scriptText.contains("echo 'this is test script'"));
    QVERIFY(scriptText.contains("End Custom Script"));

    script.close();




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
     "./" + DISTRO_DIR + "/TestOnlyC.bat",
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

void deploytest::testextraData() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "/build/TestOnlyC",
     "./" + DISTRO_DIR + "/build/QtWidgetsProject",
     "./" + DISTRO_DIR + "/build/TestQMLWidgets"});
#else
    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "/build/TestOnlyC.exe",
     "./" + DISTRO_DIR + "/build/QtWidgetsProject.exe",
     "./" + DISTRO_DIR + "/build/TestQMLWidgets.exe",
     "./" + DISTRO_DIR + "/build/TestOnlyC.bat",
     "./" + DISTRO_DIR + "/build/QtWidgetsProject.bat",
     "./" + DISTRO_DIR + "/build/TestQMLWidgets.bat"});
#endif

#ifdef Q_OS_UNIX
    comapareTree += utils.createTree(
                {"./" + DISTRO_DIR + "/build/quicknanobrowser",
                 "./" + DISTRO_DIR + "/build/webui"});
#endif


    runTestParams({"-extraData", TestBinDir, "clear",
                   "noCheckRPATH", "noCheckPATH", "noQt"}, &comapareTree);


    comapareTree = TestModule.replace(comapareTree, {
                                          {"DistributionKit/build",
                                           "DistributionKit/myExtraData/build"}});

    runTestParams({"-extraData", TestBinDir, "clear",
                   "noCheckRPATH", "noCheckPATH", "noQt",
                  "-extraDataOut", "myExtraData"}, &comapareTree);
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
     "./" + DISTRO_DIR + "/TestOnlyC.bat",
     "./" + DISTRO_DIR + "/qt.conf",
     "./" + DISTRO_DIR + "/QtWidgetsProject.exe",
     "./" + DISTRO_DIR + "/QtWidgetsProject.bat",
     "./" + DISTRO_DIR + "/TestQMLWidgets.exe",
     "./" + DISTRO_DIR + "/TestQMLWidgets.bat"
    });
#endif

#ifdef Q_OS_UNIX
    comapareTree += utils.createTree(
    {"./" + DISTRO_DIR + "/bin/quicknanobrowser",
     "./" + DISTRO_DIR + "/quicknanobrowser.sh",
     "./" + DISTRO_DIR + "/bin/webui",
     "./" + DISTRO_DIR + "/webui.sh"});
#endif

    runTestParams({"-bin", TestBinDir, "clear" , "noCheckRPATH", "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);


    QVERIFY(QFile::exists(TestBinDir + "/TestConf.json"));
    QFile::remove(TestBinDir + "/TestConf.json");

    comapareTree -= utils.createTree(
                {"./" + DISTRO_DIR + "/bin/quicknanobrowser",
                 "./" + DISTRO_DIR + "/quicknanobrowser.sh",
                 "./" + DISTRO_DIR + "/bin/webui",
                 "./" + DISTRO_DIR + "/webui.sh"});

#ifdef Q_OS_UNIX
    runTestParams({"-bin", TestBinDir + "TestOnlyC," + TestBinDir + "QtWidgetsProject," + TestBinDir + "TestQMLWidgets",
                   "clear", "noCheckRPATH", "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
#else
    runTestParams({"-bin", TestBinDir + "TestOnlyC.exe," + TestBinDir + "QtWidgetsProject.exe," + TestBinDir + "TestQMLWidgets.exe",
                   "clear" , "-libDir", "L:/never/absalut/path", "noCheckPATH", "noQt",
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
                   "clear" , "noCheckRPATH", "noCheckPATH", "noQt",
                   "-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);
#else
    runTestParams({"-bin", TestBinDir + "TestOnlyC.exe," + TestBinDir + "QtWidgetsProject.exe," + TestBinDir + "TestQMLWidgets.exe",
                   "clear" , "noCheckPATH", "noQt",
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
    QString qmake = TestQtDir + "bin/qmake";

#else

    QVERIFY(data.contains("\"bin\": ["));
    QVERIFY(data.contains("./../../../../../build/TestOnlyC.exe"));
    QVERIFY(data.contains("./../../../../../build/QtWidgetsProject.exe"));
    QVERIFY(data.contains("./../../../../../build/TestQMLWidgets.exe"));

    QVERIFY(data.contains("\"clear\": true"));
    QString qmake = TestQtDir + "bin/qmake.exe";

#endif
    runTestParams({"-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);

    QVERIFY(QuasarAppUtils::Params::isEndable("clear"));
    QVERIFY(QuasarAppUtils::Params::isEndable("bin"));

    comapareTree += TestModule.qtLibs();
    comapareTree += TestModule.qmlLibs();

    runTestParams({"-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json",
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets",
                  }, &comapareTree);

    QVERIFY(QuasarAppUtils::Params::isEndable("clear"));
    QVERIFY(QuasarAppUtils::Params::isEndable("bin"));

    QFile::remove(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");

    // Test generar string in confFile
    comapareTree = TestModule.qtLibs();

    comapareTree = TestModule.ignoreFilter(comapareTree, "/plugins/p");
#ifdef Q_OS_UNIX
    comapareTree -= utils.createTree(
    {
                    "./" + DISTRO_DIR + "/lib/libQt5EglFSDeviceIntegration.so",
                });
    auto bin =  TestBinDir + "QtWidgetsProject";
#else
    comapareTree -= utils.createTree(
    {
                    "./" + DISTRO_DIR + "/Qt5DBus.dll"

                });
    auto bin =  TestBinDir + "QtWidgetsProject.exe";
#endif

    runTestParams({"-bin", bin,
                   "-qmake", qmake,
                   "clear",
                   "-ignore", "/plugins/p",
                   "-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);


    runTestParams({"-confFile", TestBinDir + "/TestConf.json"}, &comapareTree);
    QFile::remove(TestBinDir + "/TestConf.json");

#ifdef Q_OS_UNIX
    QFile f("./" + DISTRO_DIR + "/bin/TestOnlyC");
    comapareTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/package/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/package/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/package/bin/qt.conf"
                });
    QString target1 = TestBinDir + "TestOnlyC";

#else
    QFile f("./" + DISTRO_DIR + "/TestOnlyC.exe");
    comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "/package/TestOnlyC.exe",
     "./" + DISTRO_DIR + "/package/TestOnlyC.bat",
     "./" + DISTRO_DIR + "/package/qt.conf"});
    QString target1 = TestBinDir + "TestOnlyC.exe";

#endif
    bin = target1;

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "package;TestOn",
                   "-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"}, &comapareTree);

    runTestParams({"-confFile", TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json"},
                  &comapareTree);
    QFile::remove(TestBinDir + "/../folder/For/Testing/Deploy/File/TestConf.json");

    auto file = "testCase.json";
    QVERIFY(utils.deployFile(":/testResurces/testRes/testMultiPackageConfig.json", file,
    {{"$BIN_DIR", TestBinDir.toLatin1()}}));

    comapareTree = TestModule.onlyC(DISTRO_DIR + "/Dstro1") +
            TestModule.qtLibs(DISTRO_DIR + "/Dstro2") +
            TestModule.qmlLibs(DISTRO_DIR + "/Dstro2") +
            TestModule.qtWebEngine(DISTRO_DIR + "/Dstro2") +
            TestModule.qtWebEngineWidgets(DISTRO_DIR + "/Dstro2");

#ifdef Q_OS_LINUX
    auto qmlDir = TestBinDir + "/../";
#else
    auto qmlDir = TestBinDir + "/../TestQMLWidgets";
#endif


    runTestParams({"-confFile", file,
                   "-qmlDir", "Dstro2;" + qmlDir},
                  &comapareTree);
}

void deploytest::testPackages() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QFile f("./" + DISTRO_DIR + "/bin/TestOnlyC");
    auto comapareTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/package/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/package/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/package/bin/qt.conf"
                });
    QString target1 = TestBinDir + "TestOnlyC";

#else
    QFile f("./" + DISTRO_DIR + "/TestOnlyC.exe");
    auto comapareTree = utils.createTree(
    {"./" + DISTRO_DIR + "/package/TestOnlyC.exe",
     "./" + DISTRO_DIR + "/package/TestOnlyC.bat",
     "./" + DISTRO_DIR + "/package/qt.conf"});
    QString target1 = TestBinDir + "TestOnlyC.exe";

#endif
    QString bin = target1;

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "/package/;TestOn"}, &comapareTree);

    runTestParams({"-bin", bin, "force-clear",
                   "-targetPackage", "/package/;" + QFileInfo(target1).absoluteFilePath()}, &comapareTree);

#ifdef Q_OS_UNIX
    QString target2 = TestBinDir + "TestQMLWidgets";
    QString target3 = TestBinDir + "QtWidgetsProject";

#else
    QString target2 = TestBinDir + "TestQMLWidgets.exe";
    QString target3 = TestBinDir + "QtWidgetsProject.exe";

#endif
    bin += "," + target2;
    bin += "," + target3;

    auto packageString = "package1;" + QFileInfo(target1).absoluteFilePath() + ",package2/ZzZ;" + QFileInfo(target2).absoluteFilePath();

    comapareTree = TestModule.testDistroLibs(DISTRO_DIR);

    runTestParams({"-bin", bin, "force-clear",
                   "-binOut", "/lol",
                   "-libOut", "/lolLib",
                   "-trOut", "/lolTr",
                   "-pluginOut", "/p",
                   "-qmlOut", "package2/ZzZ;/q/and/q,/q",
                   "-qmlDir", "package2/ZzZ;" + TestBinDir + "/../TestQMLWidgets",
                   "-targetPackage", packageString}, &comapareTree);


#ifdef Q_OS_UNIX

    // test a wrapers source
    QFile wraper("./" + DISTRO_DIR + "/package2/ZzZ/TestQMLWidgets.sh");

    QVERIFY(wraper.open(QIODevice::ReadOnly));
    auto data = wraper.readAll();
    wraper.close();

    wraper.setFileName(":/testResurces/testRes/TestQMLWidgets.sh");
    QVERIFY(wraper.open(QIODevice::ReadOnly));
    QVERIFY(wraper.readAll() == data);
    wraper.close();
#endif
}

void deploytest::testQt() {
    TestUtils utils;


#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "QtWidgetsProject";
    QString qmake = TestQtDir + "bin/qmake";

#else
    QString bin = TestBinDir + "QtWidgetsProject.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

#endif


    auto comapareTree = TestModule.qtLibs();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake}, &comapareTree);

    // test auto detection of detection qmake from PATH
    runTestParams({"-bin", bin, "clear", "noCheckRPATH"}, &comapareTree);

#ifdef Q_OS_UNIX
    // test auto detection of detection qmake from RPATH
    runTestParams({"-bin", bin, "clear", "noCheckPATH"}, &comapareTree);
#endif


    comapareTree = TestModule.qtWithoutTr();


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake, "noTranslations"}, &comapareTree);


    comapareTree = TestModule.qmlLibs();

#ifdef Q_OS_UNIX
    bin = TestBinDir + "TestQMLWidgets";
#else
    bin = TestBinDir + "TestQMLWidgets.exe";

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets"}, &comapareTree);


#ifdef Q_OS_UNIX

    runTestParams({"-bin", bin, "clear" ,
                   "-qmlDir", TestBinDir + "/../TestQMLWidgets"}, &comapareTree);

#endif

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
                    "./" + DISTRO_DIR + "/QtWidgetsProject.bat",
                    "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
                    "./" + DISTRO_DIR + "/libstdc++-6.dll",
                    "./" + DISTRO_DIR + "/libwinpthread-1.dll"

                });

#endif


    if (!TestQtDir.contains("Qt5")) {

#ifdef Q_OS_UNIX
        bin = TestBinDir + "QtWidgetsProject";
        qmake = TestQtDir + "bin/qmake";

#else
        bin = TestBinDir + "QtWidgetsProject.exe";
        qmake = TestQtDir + "bin/qmake.exe";

#endif
    }


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-ignore", "Qt5"}, &comapareTree);



#ifdef Q_OS_UNIX
    auto removeTree = utils.createTree({
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_hangul.so",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_openwnn.so",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_pinyin.so",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_tcime.so",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/libqtvirtualkeyboard_thai.so",
                                           "./" + DISTRO_DIR + "/plugins/platforminputcontexts/libqtvirtualkeyboardplugin.so",
                                           "./" + DISTRO_DIR + "/lib/libQt5VirtualKeyboard.so",

                                       });
#else
    auto removeTree = utils.createTree({
                                           "./" + DISTRO_DIR + "/Qt5VirtualKeyboard.dll",
                                           "./" + DISTRO_DIR + "/plugins/platforminputcontexts/qtvirtualkeyboardplugin.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_hangul.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_openwnn.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_pinyin.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_tcime.dll",
                                           "./" + DISTRO_DIR + "/plugins/virtualkeyboard/qtvirtualkeyboard_thai.dll"
                                       });

#endif

    comapareTree = TestModule.qtLibs() - removeTree;

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-ignore", "VirtualKeyboard"}, &comapareTree);

}

void deploytest::testIgnoreEnv() {



    Envirement env;
    QDir("./testTree").removeRecursively();

    QStringList ignoreTree = {
        "./testTree/test",
        "./testTree/",
        "./testTree/test1/1",
        "./testTree/test2/1/",
    };

    QStringList testTree = {
        "./testTree/test/z",
        "./testTree/z",
        "./testTree/test1/1z",
        "./testTree/test2/1/z",
    };

    createTree(ignoreTree);
    createTree(testTree);

    env.setIgnoreEnvList(ignoreTree);
    env.addEnv(ignoreTree);

    // must be empty becouse all pathes is ignored
    QVERIFY(env.size() == 0);

    env.addEnv(testTree);

    // must be equals 4 becouse all pathes is not ignored
    QVERIFY(env.size() == 4);

    // try add dublicate
    env.addEnv(testTree);

    // must be equals 4 becouse all dublicates must be ignored
    QVERIFY(env.size() == 4);

    QVERIFY(QDir("./testTree").removeRecursively());
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
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",

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
                    "./" + DISTRO_DIR + "2/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "2/bin/qt.conf",
                    "./" + DISTRO_DIR + "2/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "2/lib/libstdc++.so",
                    "./" + DISTRO_DIR + "2/lib/libgcc_s.so"
                });

#else
    comapareTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",
                    "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
                    "./" + DISTRO_DIR + "/libwinpthread-1.dll",
                    "./" + DISTRO_DIR + "/libstdc++-6.dll",

                });

    auto comapareTreeExtraLib = utils.createTree(
    {
                    "./" + DISTRO_DIR + "2/qt.conf",
                    "./" + DISTRO_DIR + "2/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "2/TestOnlyC.bat",
                    "./" + DISTRO_DIR + "2/libgcc_s_seh-1.dll",
                    "./" + DISTRO_DIR + "2/libstdc++-6.dll",

                });

#endif


    runTestParams({"-bin", bin, "clear" ,
                   "-libDir", extraPath,
                   "-recursiveDepth", "5",
                   "noCheckRPATH, noCheckPATH", "noQt"}, &comapareTree, true);

    runTestParams({"-bin", bin, "clear" ,
                   "-targetDir", "./" + DISTRO_DIR + "2",
                   "-extraLibs", "stdc,gcc",
                   "noCheckRPATH, noCheckPATH", "noQt"}, &comapareTreeExtraLib, true);

    //task #258
    //https://github.com/QuasarApp/CQtDeployer/issues/258


#ifdef Q_OS_UNIX
    comapareTreeExtraLib = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/TestOnlyC.sh",
                    "./" + DISTRO_DIR + "/bin/qt.conf",
                    "./" + DISTRO_DIR + "/bin/TestOnlyC",
                    "./" + DISTRO_DIR + "/lib/libstdc++.so",
                    "./" + DISTRO_DIR + "/lib/libgcc_s.so"
                });
    extraPath = "./" + DISTRO_DIR + "2/lib";


#else

    comapareTreeExtraLib = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",
                    "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
                    "./" + DISTRO_DIR + "/libstdc++-6.dll",

                });
    extraPath = "./" + DISTRO_DIR + "2";

#endif
    runTestParams({"-bin", bin, "clear" ,
                   "-libDir", extraPath,
                   "noCheckRPATH, noCheckPATH", "noQt"}, &comapareTreeExtraLib, true);

    QDir(extraPath).removeRecursively();

}

void deploytest::testExtraPlugins() {
    TestUtils utils;

#ifdef Q_OS_UNIX
    QString bin = TestBinDir + "QtWidgetsProject";
    QString qmake = TestQtDir + "bin/qmake";

    auto pluginTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlodbc.so",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlpsql.so",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/libqsqlite.so",
                    "./" + DISTRO_DIR + "/lib/libQt5Sql.so",
                    "./" + DISTRO_DIR + "/lib/libpq.so",

                });
#else
    QString bin = TestBinDir + "QtWidgetsProject.exe";
    QString qmake = TestQtDir + "bin/qmake.exe";

    auto pluginTree = utils.createTree(
    {
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlmysql.dll",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlodbc.dll",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlite.dll",
                    "./" + DISTRO_DIR + "/plugins/sqldrivers/qsqlpsql.dll",
                    "./" + DISTRO_DIR + "/Qt5Sql.dll",
                    "./" + DISTRO_DIR + "/libpq.dll",

                });
#endif

    auto comapareTree = TestModule.qtLibs();

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                  }, &comapareTree);


    comapareTree = comapareTree + pluginTree;


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-enablePlugins", "sqldrivers"}, &comapareTree);

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-extraPlugin", TestQtDir + "/plugins/sqldrivers"}, &comapareTree);

    comapareTree -= pluginTree;
    comapareTree -= utils.createTree(
    {
                    "./" + DISTRO_DIR + "/plugins/platforms/libqxcb.so",
                    "./" + DISTRO_DIR + "/lib/libxcb-xinerama.so.0",
                    "./" + DISTRO_DIR + "/plugins/xcbglintegrations/libqxcb-egl-integration.so",
                    "./" + DISTRO_DIR + "/plugins/xcbglintegrations/libqxcb-glx-integration.so",
                    "./" + DISTRO_DIR + "/lib/libQt5XcbQpa.so",

                });

    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "-disablePlugins", "qxcb,xcbglintegrations"}, &comapareTree);
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
     "./" + DISTRO_DIR + "Z/TestOnlyC.bat",
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
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",
                    "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
                    "./" + DISTRO_DIR + "/libstdc++-6.dll",
                    "./" + DISTRO_DIR + "/libwinpthread-1.dll",
                    "./" + DISTRO_DIR + "/msvcrt.dll",
                    "./" + DISTRO_DIR + "/qt.conf"
                });

#endif

    runTestParams({"-bin", bin, "clear" ,
                   "deploySystem"
                  }, &comapareTree);

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
                  }, &comapareTree);

    file.setFileName("./" + DISTRO_DIR + "/TestOnlyC.sh");

    QVERIFY(file.open(QIODevice::ReadOnly));

    runScript = file.readAll();
    file.close();

    QVERIFY(runScript.contains("export LD_PRELOAD="));

# else
    QString qmake = TestQtDir + "bin/qmake.exe";
    bin = TestBinDir + "QtWidgetsProject.exe";

    comapareTree += TestModule.qtLibs();

    comapareTree -= utils.createTree(
    {
                    "./" + DISTRO_DIR + "/TestOnlyC.exe",
                    "./" + DISTRO_DIR + "/TestOnlyC.bat",

                });

    comapareTree += utils.createTree(
    {
                    "./" + DISTRO_DIR + "/libgcc_s_seh-1.dll",
                    "./" + DISTRO_DIR + "/libstdc++-6.dll",
                    "./" + DISTRO_DIR + "/libwinpthread-1.dll",
                    "./" + DISTRO_DIR + "/msvcrt.dll",
                    "./" + DISTRO_DIR + "/qt.conf",
                    "./" + DISTRO_DIR + "/mpr.dll",
                    "./" + DISTRO_DIR + "/profapi.dll",
                    "./" + DISTRO_DIR + "/rpcrt4.dll",
                    "./" + DISTRO_DIR + "/shell32.dll",
                    "./" + DISTRO_DIR + "/userenv.dll",
                    "./" + DISTRO_DIR + "/uxtheme.dll",
                    "./" + DISTRO_DIR + "/version.dll",
                    "./" + DISTRO_DIR + "/ucrtbase.dll",
                    "./" + DISTRO_DIR + "/oleaut32.dll",
                    "./" + DISTRO_DIR + "/bcryptprimitives.dll",
                    "./" + DISTRO_DIR + "/msvcp_win.dll",
                    "./" + DISTRO_DIR + "/wtsapi32.dll",
                    "./" + DISTRO_DIR + "/combase.dll",

                });


#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    comapareTree += utils.createTree(
    {
                    "./" + DISTRO_DIR + "/d3d11.dll",
                    "./" + DISTRO_DIR + "/dxgi.dll",
                    "./" + DISTRO_DIR + "/win32u.dll",
                });
#endif


    runTestParams({"-bin", bin, "clear" ,
                   "-qmake", qmake,
                   "deploySystem"
                  }, &comapareTree);


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

    auto comapareTree = TestModule.testOutLibs();

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
    QVERIFY(runScript.contains("SET PATH=\"%BASE_DIR%\"\\lolLib\\;%PATH%"));
    QVERIFY(runScript.contains("call \"%BASE_DIR%\\lol\\TestQMLWidgets.exe\" %*"));


#endif

}

QTEST_APPLESS_MAIN(deploytest)

#include "tst_deploytest.moc"
